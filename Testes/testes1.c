#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(void){
    pid_t ret;

    ret = fork();
    printf(ret);
    
    return 0;
}