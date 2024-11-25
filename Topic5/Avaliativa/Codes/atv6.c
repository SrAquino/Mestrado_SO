#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 4

typedef struct
{
    int *array;
    int size;
    int core_id;
} ThreadData;

// Função para comparação de inteiros usada pelo qsort
int compare(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

// Função executada pelas threads
void *sort_array(void *arg)
{
    ThreadData *data = (ThreadData *)arg;

    // Define a afinidade da thread para o core especificado
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(data->core_id, &cpuset);

    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) != 0)
    {
        perror("Erro ao definir afinidade da thread");
        pthread_exit(NULL);
    }

    // Verifica a afinidade da thread
    cpu_set_t current_affinity;
    CPU_ZERO(&current_affinity);

    if (sched_getaffinity(0, sizeof(cpu_set_t), &current_affinity) != 0)
    {
        perror("Erro ao obter afinidade da thread");
        pthread_exit(NULL);
    }

    printf("Thread %lu executando no core %d\n", pthread_self(), data->core_id);
    for (int i = 0; i < CPU_SETSIZE; i++)
    {
        if (CPU_ISSET(i, &current_affinity))
        {
            printf("  Afinidade atual inclui core %d\n", i);
        }
    }

    // Ordena o vetor
    qsort(data->array, data->size, sizeof(int), compare);

    pthread_exit(NULL);
}

int main()
{
    int n = 100; // Tamanho dos vetores
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    int *arrays[NUM_THREADS];

    // Inicializa vetores com números aleatórios
    srand(time(NULL));
    for (int i = 0; i < NUM_THREADS; i++)
    {
        arrays[i] = (int *)malloc(n * sizeof(int));
        if (!arrays[i])
        {
            perror("Erro ao alocar memória");
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < n; j++)
        {
            arrays[i][j] = rand() % 1000;
        }
        thread_data[i].array = arrays[i];
        thread_data[i].size = n;
        thread_data[i].core_id = i % sysconf(_SC_NPROCESSORS_ONLN); // Distribui entre os cores disponíveis
    }

    // Cria threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_create(&threads[i], NULL, sort_array, &thread_data[i]) != 0)
        {
            perror("Erro ao criar thread");
            exit(EXIT_FAILURE);
        }
    }

    // Aguarda a finalização das threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Libera memória alocada
    for (int i = 0; i < NUM_THREADS; i++)
    {
        free(arrays[i]);
    }

    // printf("Ordenação concluída. Utilize o comando 'top' para verificar a utilização dos cores durante a execução.\n");

    return 0;
}
