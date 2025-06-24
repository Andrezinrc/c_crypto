#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "crypt.h"

// percorre diretorios e subdiretorios aplicando a operacao
void processDirectory(const char* dirPath, const char* keyPath, int encryptMode) {
    DIR* dir = opendir(dirPath);
    if (!dir) {
        perror("Erro ao abrir diretório");
        return;
    }

    struct dirent* entry;
    char fullPath[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

        struct stat pathStat;
        stat(fullPath, &pathStat);

        if (S_ISREG(pathStat.st_mode)) {
            printf(">> Processando: %s\n", fullPath);

            if (encryptMode) {
                if (isAlreadyEncrypted(fullPath)) {
                    printf("Ignorado (já criptografado): %s\n", fullPath);
                    continue;
                }
                if (encrypt(fullPath, keyPath)) {
                    printf("Arquivo criptografado com sucesso: %s\n", fullPath);
                } else {
                    fprintf(stderr, "Erro ao criptografar: %s\n", fullPath);
                }
            } else {
                if (!isAlreadyEncrypted(fullPath)) {
                    printf("Ignorado (não criptografado): %s\n", fullPath);
                    continue;
                }
                if (decrypt(fullPath, keyPath)) {
                    printf("Arquivo descriptografado com sucesso: %s\n", fullPath);
                } else {
                    fprintf(stderr, "Erro ao descriptografar: %s\n", fullPath);
                }
            }
        }
    }

    closedir(dir);
}
