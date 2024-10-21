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

int qnt_Leitores, qnt_Escritores, *qnt_Lendo;
sem_t *mutex_Lendo, *semaf_Escrita;

void Escrever(int escritor)
{
    for (int i = 0; i < 5; i++)
    {
        printf("Escritor %d quer escrever . . . \n\n", escritor);

        sem_wait(semaf_Escrita);

        printf("Escritor %d escrevendo . . . \n\n", escritor);
        sleep(rand() % 3 + 1);

        sem_post(semaf_Escrita);
        sleep(rand() % 3 + 1);
    }
}

void Ler(int leitor)
{
    for (int i = 0; i < 5; i++)
    {
        printf("Leitor %d se preparando para ler . . . \n\n", leitor);

        sem_wait(mutex_Lendo);

        (*qnt_Lendo)++;
        printf("Leitores lendo no momento: %d \n\n", *qnt_Lendo);

        if (*qnt_Lendo == 1)
        {
            sem_wait(semaf_Escrita);
        }

        sem_post(mutex_Lendo);
        printf("Leitor %d lendo . . . \n\n", leitor);
        sleep(rand() % 3 + 1);

        sem_wait(mutex_Lendo);
        (*qnt_Lendo)--;

        if (*qnt_Lendo == 0)
        {
            sem_post(semaf_Escrita);
        }
        sem_post(mutex_Lendo);
    }
}

int main()
{

    printf("Qual a quantidade de Leitores?:\n");
    scanf("%d", &qnt_Leitores);

    printf("Qual a quantidade de Escritores?:\n");
    scanf("%d", &qnt_Escritores);

    qnt_Lendo = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *qnt_Lendo = 0;

    mutex_Lendo = mmap(NULL, (sizeof(sem_t)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_init(mutex_Lendo, 1, 1);

    semaf_Escrita = mmap(NULL, (sizeof(sem_t)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_init(semaf_Escrita, 1, 1);

    for (int i = 0; i < qnt_Leitores; i++)
    {

        if (fork() == 0)
        {
            Ler(i);
        }
        else
        {
            // printf("Pai\n");
        }
    }
    for (int i = 0; i < qnt_Escritores; i++)
    {
        if (fork() == 0)
        {
            Escrever(i);
        }
        else
        {
            // printf("Pai\n");
        }
    }

    for (int i = 0; i < qnt_Leitores + qnt_Escritores; i++)
    {
        wait(NULL);
    }

    sem_destroy(mutex_Lendo);
    sem_destroy(semaf_Escrita);
    munmap(qnt_Lendo, sizeof(int));
    munmap(mutex_Lendo, sizeof(sem_t));
    munmap(semaf_Escrita, sizeof(sem_t));

    return 0;
}