#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>

sem_t *sem;

int test1(char *u){
    sem_wait(sem);
    printf("%s ok\n",u);
    sem_post(sem);
}

int main()
{
sem = sem_open("/test1", O_CREAT, 0666, 1);
    test1("nome");

}