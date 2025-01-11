#include <stdio.h>
#include <stdlib.h>

#define NUM_INTS 30
#define FILENAME "data.bin"

int main() {
    FILE *file = fopen(FILENAME, "wb");
    if (file == NULL) {
        perror("Erro ao criar o arquivo");
        return EXIT_FAILURE;
    }

    for (int i = 1; i <= NUM_INTS; i++) {
        fwrite(&i, sizeof(int), 1, file);
    }

    fclose(file);
    printf("Arquivo .bin '%s' criado com %d inteiros.\n", FILENAME, NUM_INTS);
    return EXIT_SUCCESS;
}
