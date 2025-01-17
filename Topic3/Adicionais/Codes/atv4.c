#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PENSANDO 0
#define COM_FOME 1
#define COMENDO 2

int qnt_filosofos;

int *state;
sem_t *mutex, *semaf;

void printStates()
{
    printf("Estados: ");
    for (int i = 0; i < qnt_filosofos; i++)
    {
        printf("%d ", state[i]);
    }
    printf("\n");
}

void teste(int filosofo)
{
    printf("Filosofo %d verificando se pode comer\n", filosofo);

    /*printf("%d %d %d\n", state[filosofo] == COM_FOME,
           state[(filosofo + qnt_filosofos - 1) % qnt_filosofos] != COMENDO,
           state[(filosofo + 1) % qnt_filosofos] != COMENDO);*/

    sem_wait(mutex);
    if (state[filosofo] == COM_FOME &&
        state[(filosofo + qnt_filosofos - 1) % qnt_filosofos] != COMENDO &&
        state[(filosofo + 1) % qnt_filosofos] != COMENDO)
    {

        printf("O %d pode!\n",filosofo);
        // printStates();
        state[filosofo] = COMENDO;
        // printStates();
        sem_post(&semaf[filosofo]);
    }
    sem_post(mutex);
}

void pensando(int filosofo)
{
    printf("Filosofo %d pensando.\n", filosofo);
    sleep(rand() % 3 + 1);
}

void comendo(int filosofo)
{
    printf("Filosofo %d comendo.\n", filosofo);
    sleep(rand() % 3 + 1);
}

void pegandoGarfo(int filosofo)
{
    printf("Filosofo %d preparando para pegar os garfos\n", filosofo);
    sem_wait(mutex);

    //printf("O estado do filosofo %d mudou DE: %d\n", filosofo, state[filosofo]);
    // printStates();
    state[filosofo] = COM_FOME;
    // printStates();
    //printf("%d (1)PARA: %d\n", filosofo, state[filosofo]);
    sem_post(mutex);

    teste(filosofo);

    sem_wait(&semaf[filosofo]);
    printf("filosofo %d pegou os garfos!\n", filosofo);
}

void largandoGarfo(int filosofo)
{
    printf("Filosofo %d largando os garfos\n", filosofo);
    sem_wait(mutex);

    //printf("O estado do filosofo %d mudou DE: %d\n", filosofo, state[filosofo]);
    // printStates();
    state[filosofo] = PENSANDO;
    // printStates();
    //printf("%d (0)PARA: %d\n", filosofo, state[filosofo]);

    //printf("Esquedo: %d Direito: %d \n", ((filosofo + qnt_filosofos - 1) % qnt_filosofos), ((filosofo + 1) % qnt_filosofos));
    sem_post(mutex);

    teste((filosofo + qnt_filosofos - 1) % qnt_filosofos);
    teste((filosofo + 1) % qnt_filosofos);
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

    printf("Qual a quantidade de filosofos?:\n");
    scanf("%d", &qnt_filosofos);

    state = mmap(NULL, (sizeof(int) * qnt_filosofos), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    mutex = mmap(NULL, (sizeof(sem_t)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_init(mutex, 1, 1);

    semaf = mmap(NULL, (sizeof(sem_t) * qnt_filosofos), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

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
        sleep(1);
    }

    munmap(state, sizeof(int) * qnt_filosofos);
    munmap(mutex, sizeof(sem_t));
    munmap(semaf, sizeof(sem_t) * qnt_filosofos);

    return 0;
}