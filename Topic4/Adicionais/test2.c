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

int main()
{
    time_t current_time = time(NULL);
    printf("SUPERVISOR TIME: %li\n", current_time);
}