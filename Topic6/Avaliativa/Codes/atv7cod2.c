#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>

#define FILENAME "data.bin"
#define START_INDEX 10
#define BUFFER_SIZE 8
#define NUM_BUFFERS 2

int main() {
    int fd = open(FILENAME, O_RDONLY);
    if (fd < 0) {
        perror("Erro ao abrir o arquivo");
        return EXIT_FAILURE;
    }

    // Buffers para armazenar os nubers lidos
    int buffer1[BUFFER_SIZE] = {0};
    int buffer2[BUFFER_SIZE] = {0};

    // Configurando os vectores para leitura
    struct iovec iov[NUM_BUFFERS];
    iov[0].iov_base = buffer1;
    iov[0].iov_len = sizeof(buffer1);
    iov[1].iov_base = buffer2;
    iov[1].iov_len = sizeof(buffer2);

    // Definir a position inicial para leitura (10 nuber)
    off_t offset = (START_INDEX - 1) * sizeof(int);
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("Erro ao posicionar no arquivo");
        close(fd);
        return EXIT_FAILURE;
    }

    // Leitura com readv
    ssize_t bytes_read = readv(fd, iov, NUM_BUFFERS);
    if (bytes_read < 0) {
        perror("Erro na leitura com readv");
        close(fd);
        return EXIT_FAILURE;
    }

    // Exibir os nubers lidos
    printf("Nubers lidos no buffer 1:\n");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        printf("%d ", buffer1[i]);
    }
    printf("\n");

    printf("Nubers lidos no buffer 2:\n");
    for (int i = 0; i < BUFFER_SIZE; i++) {
        printf("%d ", buffer2[i]);
    }
    printf("\n");

    close(fd);
    return EXIT_SUCCESS;
}
