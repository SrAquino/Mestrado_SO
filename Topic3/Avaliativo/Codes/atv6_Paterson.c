#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

volatile int contador = 0; // Contador compartilhado
volatile int desejo[2] = {0, 0}; // Desejo de entrar na seção crítica
volatile int turno; // Indica de quem é a vez

void* thread_func(void* arg) {
    int id = (long)arg; // ID da thread

    for (int i = 0; i < 5; i++) { // Cada thread incrementa 5 vezes
        // Entrada na seção crítica utilizando a solução de Peterson
        desejo[id] = 1; // Indica desejo de entrar
        turno = id; // Define a vez
        while (desejo[1 - id] == 1 && turno == id); // Espera se a outra thread deseja entrar

        // Seção crítica
        int valor = contador; // Pega o valor atual do contador
        printf("Thread %d: Contador = %d\n", id, valor);
        sched_yield(); // Cede a execução para outra thread
        contador = valor + 1; // Incrementa o contador

        // Saída da seção crítica
        desejo[id] = 0; // Indica que não deseja mais entrar
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    // Criação das threads
    pthread_create(&t1, NULL, thread_func, (void*)0);
    pthread_create(&t2, NULL, thread_func, (void*)1);

    // Espera as threads terminarem
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Valor final do contador: %d\n", contador);
    return 0;
}
