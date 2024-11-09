#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>

#define BUFFER_SIZE 1024   // Tamanho total da área de memória
#define MAX_MSG_LENGTH 256 // Tamanho máximo de uma mensagem
#define MAX_MSG_COUNT (BUFFER_SIZE / MAX_MSG_LENGTH)
#define MAX_INSTANCES 10 // Número máximo de instâncias possíveis
#define MAX_USERNAME_LENGTH 20
#define SEM_NAME "/chat_semaphore"

typedef struct
{
    char messages[MAX_MSG_COUNT][MAX_MSG_LENGTH];
    char usr_msg[MAX_MSG_COUNT][MAX_USERNAME_LENGTH];
    int read_counters[MAX_MSG_COUNT];
    time_t timestamps[MAX_MSG_COUNT];
    int write_index;
    int active_instances;
    time_t heartbeats[MAX_INSTANCES];
    char usernames[MAX_INSTANCES][MAX_USERNAME_LENGTH]; // Armazena nomes de usuários
} SharedBuffer;

SharedBuffer *shared_buffer;
sem_t *sem;
pthread_t supervisor_tid, heartbeat_tid, read_message_tid;
int instance_id;

void *supervisor_thread()
{
    while (1)
    {
        time_t current_time = time(NULL);

        sem_wait(sem); // Semáforo para sincronização

        for (int i = 0; i < MAX_INSTANCES - 1; i++)
        {
            // printf("SUPERVISOR: Verificando . . .\n");
            //  Verifica se a instância i está inativa há mais de 15 segundos
            if (shared_buffer->heartbeats[i] != 0 && difftime(current_time, shared_buffer->heartbeats[i]) > 15)
            {
                printf("SUPERVISOR: %s está AFK!\n",shared_buffer->usernames[i]);
                shared_buffer->heartbeats[i] = 0; // Marca a instância como inativa
                if (shared_buffer->active_instances > 0)
                {
                    shared_buffer->active_instances--;
                    shared_buffer->usernames[i][0] = '\0';
                }
            }
        }

        sem_post(sem); // Libera o semáforo
        // printf("SUPERVISOR: conferido!\n");
        sleep(5); // Intervalo de verificação de 5 segundos
    }
}

void *update_heartbeat(void *args)
{
    // int instance_id = *(int *)args;
    printf("%d TUM-TUM\n", instance_id);
    while (1)
    {
        sem_wait(sem);
        shared_buffer->heartbeats[instance_id] = time(NULL);
        sem_post(sem);
        // printf("%d vivo\n",instance_id);
        sleep(3); // Intervalo de atualização de 3 segundos
    }
}

void unregister_instance()
{
    sem_wait(sem);
    shared_buffer->active_instances--;
    shared_buffer->usernames[instance_id][0] = '\0';
    sem_post(sem);
}

void produce_message(char *usr)
{
    char message[MAX_MSG_LENGTH];

    while (strcmp(message, "exit") != 0)
    {
        message[0] = '\0'; 
        fgets(message, sizeof(message), stdin);
        time_t current_time = time(NULL);

        sem_wait(sem);

        // Verifica se a mensagem pode ser sobrescrita (lida por todas ou expirada)
        if (shared_buffer->read_counters[shared_buffer->write_index] == 0 ||
            difftime(current_time, shared_buffer->timestamps[shared_buffer->write_index]) > 10)
        {

            strncpy(shared_buffer->messages[shared_buffer->write_index], message, MAX_MSG_LENGTH);
            strcpy(shared_buffer->usr_msg[shared_buffer->write_index],usr);
            shared_buffer->read_counters[shared_buffer->write_index] = shared_buffer->active_instances; // Reseta o contador de leitura
            shared_buffer->timestamps[shared_buffer->write_index] = current_time;                       // Atualiza o timestamp
            shared_buffer->write_index = (shared_buffer->write_index + 1) % MAX_MSG_COUNT;              // Atualiza o índice de escrita
        }

        sem_post(sem);
    }
}

void *consume_message()
{
    int message_index = 0;
    while (1)
    {
        while (message_index == shared_buffer->write_index);
        
        sem_wait(sem);

        if (shared_buffer->read_counters[message_index] > 0)
        {
            printf("%s: %s\n", shared_buffer->usr_msg[message_index],shared_buffer->messages[message_index]);
            shared_buffer->read_counters[message_index]--; // Marca a mensagem como lida
        }

        message_index = (message_index + 1) % MAX_MSG_COUNT;
        sem_post(sem);
    }
}

void *monitor_messages(void *arg)
{
    while (1)
    {
        sem_wait(sem);
        // Lógica para verificar e processar novas mensagens
        for (int i = 0; i < MAX_MSG_COUNT; i++)
        {
            if (shared_buffer->read_counters[i] > 0)
            {
                // Processa a mensagem i (exibe e consome)
                consume_message(shared_buffer, i, sem);
            }
        }
        sem_post(sem);
        sleep(1); // Intervalo de verificação
    }
}

int register_instance_id(char *username)
{
    printf("Verificando disponibilidade do nome. . .\n");
    sem_wait(sem);
    // Verifica se o nome de usuário já está em uso
    for (int i = 0; i < MAX_INSTANCES - 1; i++)
    {
        if (strcmp(shared_buffer->usernames[i], username) == 0)
        {
            sem_post(sem);
            return -2; // Indica que o nome de usuário já existe
        }
    }
    printf("Usuário disponível!\nRegistrando . . .\n");

    // Registra um novo usuário se não estiver em uso
    for (int i = 0; i < MAX_INSTANCES - 1; i++)
    {
        if (shared_buffer->usernames[i][0] == '\0')
        { // Verifica posição vazia
            strncpy(shared_buffer->usernames[i], username, MAX_USERNAME_LENGTH);
            shared_buffer->heartbeats[i] = time(NULL); // Inicializa o *heartbeat*
            shared_buffer->active_instances++;
            sem_post(sem);
            return i; // Retorna o índice registrado
        }
        printf("Usuário online: %s\n", shared_buffer->usernames[i]);
    }
    sem_post(sem);
    return -1; // Se a capacidade máxima foi atingida
}

void cleanup_instance(int sig)
{
    printf("\nExecutando a limpeza...\n");

    pthread_cancel(supervisor_tid);
    pthread_cancel(heartbeat_tid);
    unregister_instance();
    sem_close(sem);
    sem_unlink(SEM_NAME);
    exit(0);
}

int main()
{
    signal(SIGINT, cleanup_instance);

    int fd = open("/tmp/chat", O_RDWR | O_CREAT, 0744); // Leitura e Escrita | Existe ? Cria : Edita
    if (fd == -1)
    {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    if (ftruncate(fd, sizeof(SharedBuffer)) == -1)
    {
        perror("Erro ao ajustar o tamanho da memória compartilhada");
        return 1;
    }

    shared_buffer = mmap(NULL, sizeof(SharedBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_buffer == MAP_FAILED)
    {
        perror("Erro ao mapear a memória compartilhada");
        return 1;
    }

    sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED)
    {
        perror("Erro ao criar o semáforo");
        return 1;
    }
    /*
    printf("Testando\n");
    sem_wait(sem);
    printf("Adquirido!\n");
    sem_post(sem);
    printf("Liberado!\n");
    */

    char username[MAX_USERNAME_LENGTH];

    do
    {
        printf("Digite seu nome de usuário: ");
        scanf("%s", username);

        instance_id = register_instance_id(username);
        if (instance_id == -2)
        {
            printf("Nome de usuário já em uso. Por favor, digite outro.\n");
        }
        else if (instance_id == -1)
        {
            printf("Número máximo de instâncias atingido. Não é possível registrar um novo usuário.\n");
            return 1;
        }
    } while (instance_id == -2);
    printf("Registrado com sucesso!\n");

    pthread_create(&supervisor_tid, NULL, supervisor_thread, NULL);
    pthread_create(&heartbeat_tid, NULL, update_heartbeat, (void *)&instance_id);
    pthread_create(&read_message_tid, NULL, consume_message, NULL);

    produce_message(username);

    
    cleanup_instance(1);
    
    return 0;
}
