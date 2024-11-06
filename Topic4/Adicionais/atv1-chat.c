#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>

#define TAMANHO 2000
#define NOME_ARQUIVO "/tmp/chat"
#define NOME_SEMAFORO_WRITER "/sem_writer"
#define NOME_SEMAFORO_READER "/sem_reader"

#define BUFFER_SIZE 100
#define MSG_SIZE 400

typedef struct {
    int MAIS_ANTIGA;
    int MAIS_NOVA;
    char buffer[BUFFER_SIZE][MSG_SIZE];
} chat_mem;

int fd;
chat_mem *shared_mem;
sem_t *sem_writer;
sem_t *sem_reader;

void *tfunc_escritora() {
    char msg[MSG_SIZE];
    while (1) {
        // Lê mensagem do usuário
        printf("Digite uma mensagem: ");
        fgets(msg, sizeof(msg), stdin);

        // Sincroniza com semáforo de escrita
        sem_wait(sem_writer);

        // Escreve a mensagem na posição correta
        strncpy(shared_mem->buffer[shared_mem->MAIS_NOVA], msg, MSG_SIZE);
        shared_mem->MAIS_NOVA = (shared_mem->MAIS_NOVA + 1) % BUFFER_SIZE;

        // Sincroniza a memória mapeada e libera o semáforo de leitura
        msync(shared_mem, sizeof(chat_mem), MS_SYNC);
        sem_post(sem_reader);
    }
    return NULL;
}

void *tfunc_leitora() {
    char msg[MSG_SIZE];
    while (1) {
        // Sincroniza com semáforo de leitura
        sem_wait(sem_reader);

        // Lê a mensagem mais antiga e avança o índice
        strncpy(msg, shared_mem->buffer[shared_mem->MAIS_ANTIGA], MSG_SIZE);
        shared_mem->MAIS_ANTIGA = (shared_mem->MAIS_ANTIGA + 1) % BUFFER_SIZE;

        // Exibe a mensagem na tela
        printf("Mensagem: %s\n", msg);

        // Sincroniza a memória mapeada e libera o semáforo de escrita
        msync(shared_mem, sizeof(chat_mem), MS_SYNC);
        sem_post(sem_writer);
    }
    return NULL;
}

int main() {
    pthread_t t_escritora, t_leitora;

    // Cria o arquivo e mapeia a memória compartilhada
    fd = open(NOME_ARQUIVO, O_RDWR | O_CREAT, 0666);
    ftruncate(fd, sizeof(chat_mem));
    shared_mem = mmap(NULL, sizeof(chat_mem), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Inicializa os índices de controle da memória compartilhada
    shared_mem->MAIS_ANTIGA = 0;
    shared_mem->MAIS_NOVA = 0;

    // Cria e inicializa os semáforos nomeados
    sem_writer = sem_open(NOME_SEMAFORO_WRITER, O_CREAT, 0666, BUFFER_SIZE);
    sem_reader = sem_open(NOME_SEMAFORO_READER, O_CREAT, 0666, 0);

    // Cria as threads de leitura e escrita
    pthread_create(&t_escritora, NULL, tfunc_escritora, NULL);
    pthread_create(&t_leitora, NULL, tfunc_leitora, NULL);

    // Aguarda as threads terminarem
    pthread_join(t_escritora, NULL);
    pthread_join(t_leitora, NULL);

    // Fecha e remove os semáforos e a memória compartilhada
    sem_close(sem_writer);
    sem_close(sem_reader);
    sem_unlink(NOME_SEMAFORO_WRITER);
    sem_unlink(NOME_SEMAFORO_READER);
    munmap(shared_mem, sizeof(chat_mem));
    close(fd);
    unlink(NOME_ARQUIVO);

    return 0;
}
