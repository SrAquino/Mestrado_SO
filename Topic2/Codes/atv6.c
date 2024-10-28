#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#define NUM_CANAIS 3 // Número de canais de cor RGB

// Estrutura para passar dados para as threads
typedef struct {
    unsigned char *img;   // Imagem original
    unsigned char *final; // Imagem final após o filtro
    int filtro[3][3];     // Matriz do filtro de convolução
    int w, h, c;          // Largura, altura e número de canais
    int canal;            // Canal específico para processar (R, G ou B)
} ThreadData;

// Função que aplica o filtro de convolução em um único canal
void *processa_canal(void *args) {
    ThreadData *data = (ThreadData *)args;
    int w = data->w;
    int h = data->h;
    int c = data->c;
    int canal = data->canal;
    unsigned char *img = data->img;
    unsigned char *final = data->final;
    int (*filtro)[3] = data->filtro;

    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) { // Varre toda a imagem
            int novo = 0;
            for (int ii = -1; ii < 2; ii++) {
                for (int jj = -1; jj < 2; jj++) { // Se movimente na matriz de convolução
                    int cordx = ii + i;
                    int cordy = jj + j;
                    int tmp = 0;
                    if (cordx >= 0 && cordx < w && cordy >= 0 && cordy < h) {
                        tmp = *(img + canal + (cordx * c + cordy * w * c)); // Pega a informação do pixel
                    }
                    novo += tmp * filtro[ii + 1][jj + 1]; // Aplica o filtro
                }
            }
            if (novo < 0) novo = 0;
            if (novo > 255) novo = 255;
            *(final + canal + (i * c + j * w * c)) = novo; // Cria nova imagem para o canal
        }
    }
    pthread_exit(NULL);
}

int main(void) {
    int w, h, c;
    unsigned char *img = stbi_load("imagem.jpg", &w, &h, &c, 0);
    
    if (img == NULL) {
        printf("Nao foi possivel ler a imagem\n");
        exit(1);
    }

    unsigned char *final = malloc(w * h * c); // Alocando espaço para a nova imagem
    int filtro[3][3] = { {-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1} };
    
    pthread_t threads[NUM_CANAIS]; // Array de threads
    ThreadData thread_data[NUM_CANAIS]; // Dados para cada thread

    // Inicializa os dados e cria uma thread para cada canal de cor
    for (int canal = 0; canal < c; canal++) {
        thread_data[canal].img = img;
        thread_data[canal].final = final;
        thread_data[canal].w = w;
        thread_data[canal].h = h;
        thread_data[canal].c = c;
        thread_data[canal].canal = canal;
        memcpy(thread_data[canal].filtro, filtro, sizeof(filtro));
        pthread_create(&threads[canal], NULL, processa_canal, (void *)&thread_data[canal]);
    }

    // Espera todas as threads terminarem
    for (int canal = 0; canal < c; canal++) {
        pthread_join(threads[canal], NULL);
    }

    // Salva a nova imagem
    stbi_write_jpg("imagem2.jpg", w, h, c, final, 100);

    stbi_image_free(img);
    free(final);
    
    return 0;
}
