// Atividade 9

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

#define NUM_FILHOS 3
#define TEMPO_EXECUCAO 10 // 10 segundos

void imprimir_numeros(int id) {
    int numero = 1;
    while (1) {
        printf("Filho %d (PID: %d) imprimindo: %d\n", id, getpid(), numero++);
        sleep(1); // Espera 1 segundo
    }
}

int main() {
    pid_t filhos[NUM_FILHOS];

    for (int i = 0; i < NUM_FILHOS; i++) {
        filhos[i] = fork();

        if (filhos[i] < 0) {
            perror("Erro ao criar filho");
            exit(1);
        }
        if (filhos[i] == 0) {
            // Processo filho
            imprimir_numeros(i);
            exit(0); // Termina o filho após imprimir
        }
    }

    sleep(5); // Espera 5 segundo

    // Pausa todos os filhos
    for (int i = 0; i < NUM_FILHOS; i++) {
        kill(filhos[i], SIGSTOP);
    }

    // Executa os filhos em sequência
    for (int i = 0; i < NUM_FILHOS; i++) {
        // Libera o filho atual
        kill(filhos[i], SIGCONT);
        sleep(TEMPO_EXECUCAO); // Espera 10 segundos

        // Para o filho atual
        kill(filhos[i], SIGSTOP);
    }

    // Espera os filhos terminarem (se chegarem ao final)
    for (int i = 0; i < NUM_FILHOS; i++) {
        waitpid(filhos[i], NULL, 0);
    }

    return 0;
}
