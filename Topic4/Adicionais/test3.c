#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handle_sigint(int sig) {
    printf("\nRecebi o sinal SIGINT (Ctrl+C). Executando a limpeza...\n");
    // Adicione aqui o código que deve ser executado quando Ctrl+C for pressionado
    exit(0); // Opcional: encerra o programa após o tratamento
}

int main() {
    // Configura a função handle_sigint para lidar com o sinal SIGINT
    signal(SIGINT, handle_sigint);

    printf("Pressione Ctrl+C para interromper o programa.\n");

    while (1) {
        printf("Trabalhando...\n");
        sleep(1); // Simula o trabalho do programa
    }

    return 0;
}
