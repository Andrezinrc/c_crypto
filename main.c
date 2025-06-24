#include <stdio.h>
#include <string.h>
#include "crypt.h"
#include "processDirectory.h"
#include <dirent.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s encrypt <arquivo|pasta> <chave>\n", argv[0]);
        return 1;
    }

    const char *operation = argv[1];
    const char *file = argv[2];
    const char *fileKey = argv[3];

    unsigned char key[KEY_SIZE];
    const char* keyPath = fileKey;

    if (loadKey(keyPath, key, KEY_SIZE)) {
        printf("Chave carregada com sucesso!\n");
    } else {
        fprintf(stderr, "Falha ao carregar a chave.\n");
        return 1;
    }

    struct stat pathStat;
    stat(file, &pathStat);

    // se for diretorio, processa recursivamente
    if (S_ISDIR(pathStat.st_mode)) {
        processDirectory(file, fileKey, strcmp(operation, "encrypt") == 0);
    } else if (S_ISREG(pathStat.st_mode)) {
        if (strcmp(operation, "encrypt") == 0) {
            if (isAlreadyEncrypted(file)) {
                printf("Ignorado (já criptografado): %s\n", file);
            } else {
                if (encrypt(file, fileKey)) {
                    printf("Arquivo criptografado com sucesso!\n");
                } else {
                    fprintf(stderr, "Erro ao criptografar o arquivo.\n");
                }
            }
        } else if (strcmp(operation, "decrypt") == 0) {
            if (!isAlreadyEncrypted(file)) {
                printf("Ignorado (não criptografado): %s\n", file);
            } else {
                if (decrypt(file, fileKey)) {
                    printf("Arquivo descriptografado com sucesso!\n");
                } else {
                    fprintf(stderr, "Erro ao descriptografar o arquivo.\n");
                }
            }
        } else {
            fprintf(stderr, "Operação inválida: %s\n", operation);
            return 1;
        }
    } else {
        fprintf(stderr, "Caminho inválido.\n");
        return 1;
    }

    return 0;
}
