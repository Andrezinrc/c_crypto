#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "crypt.h"
#include "processDirectory.h"
#include "colors.h"
#include "help.h"

int main(int argc, char *argv[]) {
    printf("\033[2J\033[H");
    printf(BLUE "======================================\n" RESET);
    printf(BLUE "          SYMMETRIC CRYPTO 1.0\n" RESET);
    printf(BLUE "      Simple File Encryption Tool\n" RESET);
    printf(BLUE "======================================\n" RESET);
    printf(BLUE "   Desenvolvido por Andrecode ©2025\n\n" RESET);

    if (argc < 2 || strcmp(argv[1], "help") == 0) {
        printUsage(argv[0]);
        return 1;
    }

    const char *operation = argv[1];

    // generateKey
    if (strcmp(operation, "generateKey") == 0) {
        if (argc != 3) {
            fprintf(stderr, RED "Uso: %s generateKey <arquivo-chave>\n" RESET, argv[0]);
            return 1;
        }

        const char *keyFilePath = argv[2];
        generateKey(keyFilePath);
        return 0;
    }

    // createFile
    if (strcmp(operation, "createFile") == 0) {
        if (argc != 5) {
            fprintf(stderr, RED "Uso: %s createFile <arquivo.txt> <chave> <conteúdo>\n" RESET, argv[0]);
            return 1;
        }

        const char *filename = argv[2];
        const char *keyfile = argv[3];
        const char *content = argv[4];

        create_and_encrypt_file(filename, keyfile, content);
        printf(GREEN "Arquivo criado e criptografado com sucesso!\n" RESET);
        return 0;
    }

    // encrypt / decrypt
    if (strcmp(operation, "encrypt") != 0 && strcmp(operation, "decrypt") != 0) {
        fprintf(stderr, RED "Operação inválida: %s\n" RESET, operation);
        return 1;
    }

    if (argc != 4) {
        fprintf(stderr, RED "Uso: %s %s <arquivo|pasta> <chave>\n" RESET, argv[0], operation);
        return 1;
    }

    const char *targetPath = argv[2];
    const char *keyPath = argv[3];

    // carrega a chave
    unsigned char key[KEY_SIZE];
    if (!loadKey(keyPath, key, KEY_SIZE)) {
        fprintf(stderr, RED "Falha ao carregar a chave.\n" RESET);
        return 1;
    }
    printf(GREEN "Chave carregada com sucesso!\n" RESET);

    // verifica o tipo de caminho
    struct stat pathStat;
    if (stat(targetPath, &pathStat) != 0) {
        fprintf(stderr, RED "Erro ao acessar o caminho: " RESET);
        perror("");
        return 1;
    }

    int encrypting = strcmp(operation, "encrypt") == 0;

    if (S_ISDIR(pathStat.st_mode)) {
        processDirectory(targetPath, keyPath, encrypting);
    } else if (S_ISREG(pathStat.st_mode)) {
        if (encrypting) {
            if (isAlreadyEncrypted(targetPath)) {
                printf(RED "Ignorado (já criptografado): %s\n" RESET, targetPath);
            } else if (encrypt(targetPath, keyPath)) {
                printf(GREEN "Arquivo criptografado com sucesso!\n" RESET);
            } else {
                fprintf(stderr, RED "Erro ao criptografar o arquivo.\n" RESET);
            }
        } else {
            if (!isAlreadyEncrypted(targetPath)) {
                printf(RED "Ignorado (não criptografado): %s\n" RESET, targetPath);
            } else if (decrypt(targetPath, keyPath)) {
                printf(GREEN "Arquivo descriptografado com sucesso!\n" RESET);
            } else {
                fprintf(stderr, RED "Erro ao descriptografar o arquivo.\n" RESET);
            }
        }
    } else {
        fprintf(stderr, RED "Caminho inválido (não é arquivo nem diretório).\n" RESET);
        return 1;
    }

    return 0;
}
