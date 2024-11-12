#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>

#define BUFFER_SIZE 10 // Tamanho do buffer compartilhado

typedef struct
{
    char buffer[BUFFER_SIZE];
    int in;
    int out;
} shared_memory;

int main()
{

    shared_memory *shared = mmap(NULL, sizeof(shared_memory),
                                 PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared == MAP_FAILED)
    {
        perror("mmap");
        exit(1);
    }

    shared->in = 0;
    shared->out = 0;

    sem_t *mutex = sem_open("/mute", O_CREAT | O_EXCL, 0644, 1);

    if (mutex == SEM_FAILED)
    {
        perror("sem_open");
        exit(1);
    }

    pid_t producer_pid = fork();
    if (producer_pid == 0)
    {

        srand(time(NULL));
        while (1)
        {
            sleep(2);

            char letter = 'A' + (rand() % 26);

            sem_wait(mutex);

            shared->buffer[shared->in] = letter;
            printf("Produtor: letra %c adicionada na entrada %d\n", letter, shared->in);
            shared->in = (shared->in + 1) % BUFFER_SIZE;

            sem_post(mutex);
        }
    }

    pid_t consumer_pid = fork();
    if (consumer_pid == 0)
    {

        while (1)
        {
            sleep(3);

            sem_wait(mutex);

            char letter = shared->buffer[shared->out];
            printf("Consumidor: letra %c consumida da entrada %d\n", letter, shared->out);
            shared->out = (shared->out + 1) % BUFFER_SIZE;

            sem_post(mutex);
        }
    }

    // Processo pai
    wait(NULL);
    wait(NULL);

    // Limpeza
    sem_unlink("/empty");
    sem_unlink("/full");
    sem_unlink("/mutex");
    sem_close(mutex);
    munmap(shared, sizeof(shared_memory));

    return 0;
}
