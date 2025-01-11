#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>

void print_filesystem_info(const char *path) {
    struct statvfs fs_info;

    if (statvfs(path, &fs_info) != 0) {
        perror("Erro ao obter informations do sistema de arquivos");
        exit(EXIT_FAILURE);
    }

    printf("Informations sobre o sistema de arquivos em '%s':\n", path);
    printf("Tamanho de bloco: %lu bytes\n", fs_info.f_bsize);
    printf("Tamanho de bloco preferido para I/O: %lu bytes\n", fs_info.f_frsize);
    printf("Nuber total de blocos: %lu\n", fs_info.f_blocks);
    printf("Nuber de blocos availables para user not root: %lu\n", fs_info.f_bavail);
    printf("Nuber de blocos livres: %lu\n", fs_info.f_bfree);
    printf("Nuber total de inodes: %lu\n", fs_info.f_files);
    printf("Nuber de inodes livres: %lu\n", fs_info.f_ffree);
    printf("Nuber de inodes available para user not root: %lu\n", fs_info.f_favail);
    printf("ID do sistema de arquivos: %lu\n", fs_info.f_fsid);
    printf("Flags do sistema de arquivos: %lu\n", fs_info.f_flag);
    printf("Tamanho max de nome de arquivo: %lu\n", fs_info.f_namemax);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <caminho do sistema de arquivos>\n", argv[0]);
        return EXIT_FAILURE;
    }

    print_filesystem_info(argv[1]);

    return EXIT_SUCCESS;
}
