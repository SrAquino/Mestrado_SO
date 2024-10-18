#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_FILHOS 4
#define NUM_THREADS 3

void *thread_function(void *args)
{
    int id = *((int *)args);

    int sleep_time = rand() % 9 + 2; // Tempo de sono entre 2 e 10 segundos
    printf("Thread %d dormindo por %d segundos.\n", (int)id, sleep_time);
    sleep(sleep_time);
    printf("Thread %d terminando.\n", id);
    pthread_exit(NULL);
}

int main()
{
    int pipes[NUM_FILHOS][2]; // Pipes para comunicação com os filhos
    int thread_args[NUM_THREADS];

    for (int i = 0; i < NUM_FILHOS; i++)
    {
        pipe(pipes[i]);
        close(pipes[i][0]); // Fecha o lado de leitura do pipe

        if (fork() == 0)
        { // Filho
            pthread_t threads[NUM_THREADS];

            for (int j = 0; j < NUM_THREADS; j++)
            {
                thread_args[j] = j;
                pthread_create(&threads[j], NULL, thread_function, (void *)&thread_args[j]);
            }

            for (int j = 0; j < NUM_THREADS; j++)
            {
                pthread_join(threads[j], NULL); // Esperando as threads terminarem
            }

            pid_t pid_filho = getpid();

            write(pipes[i][1], &pid_filho, sizeof(pid_t));
            close(pipes[i][1]); // Fechando o lado de escrita do pipe
            exit(EXIT_SUCCESS);
        }
        else
        {                       // Pai
            close(pipes[i][1]); // Fecha o lado de escrita do pipe
        }
    }

    pid_t pids[NUM_FILHOS];

    for (int i = 0; i < NUM_FILHOS; i++)
    {
        // Lendo PIDs dos filhos
        read(pipes[i][0], &pids[i], sizeof(pid_t));
    }

    // Esperando os filhos terminarem
    for (int i = 0; i < NUM_FILHOS; i++)
    {
        waitpid(pids[i], NULL, 0);
    }

    printf("PIDs dos processos filhos:\n");
    for (int i = 0; i < NUM_FILHOS; i++)
    {
        printf("Filho %d: PID = %d\n", i + 1, pids[i]);
    }

    return 0;
}
