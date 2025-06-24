#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "crypt.h"
#include "processDirectory.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso:\n");
        fprintf(stderr, "  %s generateKey <arquivo-chave>\n", argv[0]);
        fprintf(stderr, "  %s encrypt <arquivo|pasta> <chave>\n", argv[0]);
        fprintf(stderr, "  %s decrypt <arquivo|pasta> <chave>\n", argv[0]);
        return 1;
    }

    const char *operation = argv[1];

    // generateKey
    if (strcmp(operation, "generateKey") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Uso: %s generateKey <arquivo-chave>\n", argv[0]);
            return 1;
        }

        const char *keyFilePath = argv[2];
        generateKey(keyFilePath);
        return 0;
    }

    // encrypt / decrypt
    if (strcmp(operation, "encrypt") != 0 && strcmp(operation, "decrypt") != 0) {
        fprintf(stderr, "Operação inválida: %s\n", operation);
        return 1;
    }

    if (argc != 4) {
        fprintf(stderr, "Uso: %s %s <arquivo|pasta> <chave>\n", argv[0], operation);
        return 1;
    }

    const char *targetPath = argv[2];
    const char *keyPath = argv[3];

    // carrega a chave
    unsigned char key[KEY_SIZE];
    if (!loadKey(keyPath, key, KEY_SIZE)) {
        fprintf(stderr, "Falha ao carregar a chave.\n");
        return 1;
    }
    printf("Chave carregada com sucesso!\n");

    // verifica o tipo de caminho
    struct stat pathStat;
    if (stat(targetPath, &pathStat) != 0) {
        perror("Erro ao acessar o caminho");
        return 1;
    }

    int encrypting = strcmp(operation, "encrypt") == 0;

    if (S_ISDIR(pathStat.st_mode)) {
        processDirectory(targetPath, keyPath, encrypting);
    } else if (S_ISREG(pathStat.st_mode)) {
        if (encrypting) {
            if (isAlreadyEncrypted(targetPath)) {
                printf("Ignorado (já criptografado): %s\n", targetPath);
            } else if (encrypt(targetPath, keyPath)) {
                printf("Arquivo criptografado com sucesso!\n");
            } else {
                fprintf(stderr, "Erro ao criptografar o arquivo.\n");
            }
        } else {
            if (!isAlreadyEncrypted(targetPath)) {
                printf("Ignorado (não criptografado): %s\n", targetPath);
            } else if (decrypt(targetPath, keyPath)) {
                printf("Arquivo descriptografado com sucesso!\n");
            } else {
                fprintf(stderr, "Erro ao descriptografar o arquivo.\n");
            }
        }
    } else {
        fprintf(stderr, "Caminho inválido (não é arquivo nem diretório).\n");
        return 1;
    }

    return 0;
}
