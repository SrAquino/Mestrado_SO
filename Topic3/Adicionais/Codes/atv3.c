/*
gcc -o atv3 atv3.c -lpthread
./atv3
*/

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <unistd.h>

volatile int contador = 0;
pthread_mutex_t mutex;

volatile int turno;

void *incrementa_contador(void *arg)
{
    int id = *(int *)arg;

    for (int i = 0; i < 10; i++)
    {
        while (1)
        {
            pthread_mutex_lock(&mutex);
            if (turno == id)
            {
                contador++;
                printf("%d: %d\n", id, contador);
                turno = (turno + 1) % 2;
                pthread_mutex_unlock(&mutex);
                break; 
            }
            pthread_mutex_unlock(&mutex);
        }
    }

    return NULL;
}

int main()
{
    pthread_t threads[2];
    pthread_mutex_init(&mutex, NULL);
    int ids[2] = {0, 1};

    for (int i = 0; i < 2; i++)
    {
        if (pthread_create(&threads[i], NULL, incrementa_contador, &ids[i]) != 0)
        {
            perror("Falha ao criar thread");
            return 1;
        }
    }

    for (int i = 0; i < 2; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    return 0;
}
