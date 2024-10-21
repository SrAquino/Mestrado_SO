#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_ITEMS 20

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;
pthread_mutex_t mutex;
pthread_cond_t not_full;
pthread_cond_t not_empty;

void *producer(void *arg)
{
    for (int i = 0; i < NUM_ITEMS; i++)
    {
        int item = rand() % 100;

        pthread_mutex_lock(&mutex);

        while ((in + 1) % BUFFER_SIZE == out)
        {
            pthread_cond_wait(&not_full, &mutex);
        }

        buffer[in] = item;
        printf("Produzido: %d\n", item);
        in = (in + 1) % BUFFER_SIZE;

        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 2);
    }
    //printf("Fim de producao\n");
    return NULL;
}

void *consumer(void *arg)
{
    int items_consumed;

    for (int i = 0; i < NUM_ITEMS; i++)
    {
        pthread_mutex_lock(&mutex);

        while (in == out)
        {
            pthread_cond_wait(&not_empty, &mutex);
        }

        items_consumed = 0;
        while (in != out)
        {
            int item = buffer[out];
            printf("Consumido: %d\n", item);
            out = (out + 1) % BUFFER_SIZE;
            items_consumed++;
        }

        printf("Total consumido nessa rodada: %d\n", items_consumed);

        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 5);
    }
    printf("Fim de consumo\n");
    return NULL;
}

int main()
{
    pthread_t prod_thread, cons_thread;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&not_full, NULL);
    pthread_cond_init(&not_empty, NULL);

    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&not_full);
    pthread_cond_destroy(&not_empty);

    return 0;
}
