#include <stdio.h>
#include <string.h>
#include "crypt.h"

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s encrypt <arquivo> <chave>\n", argv[0]);
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
    }

    if (strcmp(operation, "encrypt") == 0) {
        if (encrypt(file, fileKey)) {
            printf("Arquivo criptografado com sucesso!\n");
        } else {
            fprintf(stderr, "Erro ao criptografar o arquivo.\n");
        }
    } else if (strcmp(operation, "decrypt") == 0) {
        // decrypt
    } else {
        fprintf(stderr, "Operação inválida: %s\n", operation);
        return 1;
    }

    return 0;
}
