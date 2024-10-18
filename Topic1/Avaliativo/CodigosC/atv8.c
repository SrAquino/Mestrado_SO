// Atividade 8
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_FILHOS 5
#define MIN_NUM 0
#define MAX_NUM 200

int main() {
    pid_t pids[NUM_FILHOS];     // Armazena os PIDs dos filhos
    int pipes[NUM_FILHOS][2];   // Pipes para comunicação com os filhos
    int i;

    for (i = 0; i < NUM_FILHOS; i++) {
        pipe(pipes[i]);
    }

    for (i = 0; i < NUM_FILHOS; i++){
        pids[i] = fork();

        if (pids[i] < 0) {
            printf("Erro ao criar processo.\n");
        } else if (pids[i] == 0) {
            // Filho
            close(pipes[i][0]);     // Fecha o lado de leitura do pipe

            srand(time(NULL) ^ (getpid() << 16)); // Semente aleatória
            int numero = rand() % (MAX_NUM + 1);

            // Envia o número para o pai
            write(pipes[i][1], &numero, sizeof(numero));
            close(pipes[i][1]); // Fecha o lado de escrita do pipe
            exit(EXIT_SUCCESS); // Termina o filho
        } else {
            // Pai
            close(pipes[i][1]); // Fecha o lado de escrita do pipe
        }
    }

    int menor_numero = MAX_NUM + 1; // Inicializa com um valor alto
    pid_t pid_menor = -1; // PID do filho correspondente ao menor número

    int numero_recebido;

    for (i = 0; i < NUM_FILHOS; i++) {
        wait(NULL); // Espera todos os filhos terminarem
    }
    
    for (i = 0; i < NUM_FILHOS; i++) {
        read(pipes[i][0], &numero_recebido, sizeof(numero_recebido)); // Recebe o número do filho
        printf("Filho %d enviou o número: %d\n", pids[i], numero_recebido);

        // Verifica se é o menor número
        if (numero_recebido < menor_numero) {
            menor_numero = numero_recebido;
            pid_menor = pids[i];
        }
        close(pipes[i][0]); // Fecha o lado de leitura do pipe
    }

    printf("Menor número recebido: %d, enviado pelo filho com PID: %d\n", menor_numero, pid_menor);
 
    return 0;
}
