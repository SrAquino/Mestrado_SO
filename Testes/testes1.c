#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(void){
    pid_t ret;

    ret = fork();

    if (ret < 0) {
        // Erro ao criar o processo
        printf("Erro ao criar processo.\n");
    } else if (ret == 0) {
        // Código executado pelo processo filho
        printf("Este é o processo filho. PID = %d\n", getpid());
    } else {
        // Código executado pelo processo pai
        printf("Este é o processo pai. PID do filho = %d\n", ret);
    }

    return 0;
}
