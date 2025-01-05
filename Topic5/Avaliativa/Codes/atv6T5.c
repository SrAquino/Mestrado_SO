#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <time.h>

#define N 100000 // Tamanho dos vetores
#define NUM_THREADS 4

typedef struct
{
    int *vetor;
    size_t tamanho;
    int core_id;
} ThreadArgs;

void gera_vetor(int *vetor, size_t tamanho)
{
    for (size_t i = 0; i < tamanho; i++)
    {
        vetor[i] = rand() % 10000;
    }
}

void bubble_sort(int *vetor, size_t tamanho, int core_id)
{
    printf("Thread ordenando no core %d...\n", core_id);
    for (size_t i = 0; i < tamanho - 1; i++)
    {
        for (size_t j = 0; j < tamanho - i - 1; j++)
        {
            if (vetor[j] > vetor[j + 1])
            {
                int temp = vetor[j];
                vetor[j] = vetor[j + 1];
                vetor[j + 1] = temp;
            }
        }
        if (i % (tamanho / 10) == 0)
        { // Mostrar progresso a cada 10% do vetor ordenado
            int current_core = sched_getcpu();
            printf("Progresso: %.2f%%, executando no core atual %d (esperado: %d)\n", 
                   (i * 100.0) / tamanho, current_core, core_id);
        }
    }
    printf("Core %d: Ordering completed.\n", core_id);
}

void *ordena_vetor(void *args)
{
    ThreadArgs *argumentos = (ThreadArgs *)args;
    cpu_set_t cpuset;

    // Configurar a afinidade da thread ao core especificado
    CPU_ZERO(&cpuset);
    CPU_SET(argumentos->core_id, &cpuset);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) != 0)
    {
        perror("Erro ao configurar afinidade");
        pthread_exit(NULL);
    }

    // Verificar a afinidade configurada
    CPU_ZERO(&cpuset);
    if (sched_getaffinity(0, sizeof(cpu_set_t), &cpuset) != 0)
    {
        perror("Erro ao obter afinidade");
        pthread_exit(NULL);
    }

    if (CPU_ISSET(argumentos->core_id, &cpuset))
    {
        printf("Thread no core %d configurada corretamente.\n", argumentos->core_id);
    }
    else
    {
        printf("Erro: Thread no core %d not configurada corretamente.\n", argumentos->core_id);
    }

    // Ordenar o vetor usando Bubble Sort
    bubble_sort(argumentos->vetor, argumentos->tamanho, argumentos->core_id);
    pthread_exit(NULL);
}

int main()
{
    srand(time(NULL));
    pthread_t threads[NUM_THREADS];
    ThreadArgs args[NUM_THREADS];
    int *vetores[NUM_THREADS];

    // Verificar o number de cores available
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Number of availables cores: %d\n", num_cores);

    // Criar e inicializar os vetores
    for (int i = 0; i < NUM_THREADS; i++)
    {
        vetores[i] = (int *)malloc(N * sizeof(int));
        if (!vetores[i])
        {
            perror("Erro ao alocar memory");
            return EXIT_FAILURE;
        }
        gera_vetor(vetores[i], N);

        args[i].vetor = vetores[i];
        args[i].tamanho = N;
        args[i].core_id = i % num_cores; // Garantir distribution entre os cores
    }

    // Criar threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_create(&threads[i], NULL, ordena_vetor, &args[i]) != 0)
        {
            perror("Erro ao criar thread");
            return EXIT_FAILURE;
        }
    }

    // Aguardar completion das threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Liberar memory alocada
    for (int i = 0; i < NUM_THREADS; i++)
    {
        free(vetores[i]);
    }

    return EXIT_SUCCESS;
}
