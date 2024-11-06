#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

volatile int contador = 0; // Contador compartilhado

void* thread_func(void* arg) {
    for (int i = 0; i < 5; i++) { // Cada thread incrementa 5 vezes
        int valor = contador; // Pega o valor atual do contador
        printf("Thread %ld: Contador = %d\n", (long)arg, valor);
        sched_yield(); // Cede a execução para outra thread
        contador = valor + 1; // Incrementa o contador
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    // Criação das threads
    pthread_create(&t1, NULL, thread_func, (void*)1);
    pthread_create(&t2, NULL, thread_func, (void*)2);

    // Espera as threads terminarem
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Valor final do contador: %d\n", contador);
    return 0;
}
