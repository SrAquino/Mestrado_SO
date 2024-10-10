/* Implementar o problema dos filósofos glutões
 * com semáfaforos nomeados.
 * Cada filósofo será implementado como um processo.
 * Para o controle de concorrência, serão utilizados uma série de semáforos nomeados.
 * O programa deverá receber um parâmetro que é o número de filósofos a serem criados.
 * Para criação de processos, a chamada fork deve ser utilizada.
 */

#include <stdio.h>
#include <stdlib.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>    /* For O_* constants */
#include <sys/stat.h> /* For mode constants */

#define PENSANDO 0
#define COM_FOME 1
#define COMENDO 2

int qnt_filosofos;

int *state;
sem_t *mutex, *semaf;

void teste(int filosofo)
{
    printf("Filosofo %d verificando se pode comer\n", filosofo);

    if (state[filosofo] == COM_FOME &&
        state[(filosofo + N - 1) % N] != COMENDO &&
        state[(filosofo + 1) % N] != COMENDO)
    {

        printf("Ele pode!\n");
        state[filosofo] = COMENDO;
        sem_post(&semaf[filosofo]);
        printf("desbloqueado filosofo %d \n", filosofo);
    }
}

void pensando(int filosofo)
{
    printf("Filósofo %d está pensando.\n", filosofo);
    sleep(rand() % 3 + 1);
}

void comendo(int filosofo)
{
    printf("Filósofo %d está comendo.\n", filosofo);
    sleep(rand() % 3 + 1);
}

void pegandoGarfo(int filosofo)
{
    printf("Filosofo %d está pegando os garfos\n", filosofo);
    sem_wait(mutex);

    state[filosofo] = COM_FOME;

    teste(filosofo);

    sem_post(mutex);
    sem_wait(&semaf[filosofo]);
    printf("sem_wait desbloqueado\n");
}

void largandoGarfo(int filosofo)
{
    printf("Filosofo %d está largando os garfos\n", filosofo);
    sem_wait(mutex);

    state[filosofo] = PENSANDO;

    printf("Esquedo: %d Direito: %d \n", ((filosofo + N - 1) % N), ((filosofo + 1) % N));
    teste((filosofo + N - 1) % N);
    teste((filosofo + 1) % N);

    sem_post(mutex);
}

void rotinaDoFilosofo(int filosofo)
{
    while (1)
    {
        pensando(filosofo);
        pegandoGarfo(filosofo);
        comendo(filosofo);
        largandoGarfo(filosofo);
    }
}

int main()
{

    printf("Qual a quantidade de filósofos?:\n");
    scanf("%d", &qnt_filosofos);

    state = mmap(NULL, (sizeof(int) * qnt_filosofos), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    mutex = mmap(NULL, (sizeof(sem_t)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_init(mutex, 0, 1);

    semaf = mmap(NULL, (sizeof(sem_t) * qnt_filosofos), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // Inicializa os semáforos dos filósofos
    for (int i = 0; i < qnt_filosofos; i++)
    {
        sem_init(&semaf[i], 1, 0);
    }

    for (int i = 0; i < qnt_filosofos; i++)
    {
        if (fork() == 0)
        {
            rotinaDoFilosofo(i);
        }
    }

    while (1)
    {
        sleep(10); // Mantém o processo principal vivo
    }

    munmap(state, sizeof(int) * N);
    munmap(mutex, sizeof(sem_t));
    munmap(semaf, sizeof(sem_t) * N);

    return 0;
}