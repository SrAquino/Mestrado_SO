#define PENSANDO 0
#define COM_FOME 1
#define COMENDO 2

int N;

int *state;
sem_t *mutex, *semaf;

void teste(int filosofo) {
    // Verifica se o filósofo pode comer
    if (state[filosofo] == COM_FOME &&
        state[(filosofo + N - 1) % N] != COMENDO && 
        state[(filosofo + 1) % N] != COMENDO) {
        state[filosofo] = COMENDO;
        sem_post(&semaf[filosofo]); // Desbloqueia o filósofo para comer
    }
}

void pensando(int filosofo) {
    printf("Filósofo %d está pensando.\n", filosofo);
    sleep(rand() % 3 + 1);
}

void comendo(int filosofo) {
    printf("Filósofo %d está comendo.\n", filosofo);
    sleep(rand() % 3 + 1);
}

void pegandoGarfo(int filosofo) {
    sem_wait(mutex); // Entrar na região crítica
    state[filosofo] = COM_FOME;
    teste(filosofo); // Testa se o filósofo pode comer
    sem_post(mutex); // Sair da região crítica
    sem_wait(&semaf[filosofo]); // Bloqueia até que o filósofo possa comer
}

void largandoGarfo(int filosofo) {
    sem_wait(mutex); // Entrar na região crítica
    state[filosofo] = PENSANDO;
    // Verifica se os filósofos adjacentes podem comer
    teste((filosofo + N - 1) % N); // Testa o filósofo da esquerda
    teste((filosofo + 1) % N); // Testa o filósofo da direita
    sem_post(mutex); // Sair da região crítica
}

void rotinaDoFilosofo(int filosofo) {
    while (1) {
        pensando(filosofo);
        pegandoGarfo(filosofo);
        comendo(filosofo);
        largandoGarfo(filosofo);
    }
}

int main() {
    printf("Qual a quantidade de filósofos?:\n");
    scanf("%d", &N);

    state = mmap(NULL, sizeof(int) * N, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    semaf = mmap(NULL, sizeof(sem_t) * N, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    sem_init(mutex, 1, 1); // Inicializa o mutex

    // Inicializa os semáforos dos filósofos
    for (int i = 0; i < N; i++) {
        sem_init(&semaf[i], 1, 0);
    }

    for (int i = 0; i < N; i++) {
        if (fork() == 0) {
            rotinaDoFilosofo(i);
            exit(0);
        }
    }

    while (1) {
        sleep(10); // Mantém o processo principal vivo
    }

    munmap(state, sizeof(int) * N);
    munmap(mutex, sizeof(sem_t));
    munmap(semaf, sizeof(sem_t) * N);

    return 0;
}
