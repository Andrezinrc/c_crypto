#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "crypt.h"
#include "colors.h"

// percorre diretorios e subdiretorios aplicando a operacao
void processDirectory(const char* dirPath, const char* keyPath, int encryptMode) {
    DIR* dir = opendir(dirPath);
    if (!dir) {
        fprintf(stderr, RED "Erro ao abrir diretório: %s\n" RESET, dirPath);
        return;
    }

    struct dirent* entry;
    char fullPath[1024];

    while ((entry = readdir(dir)) != NULL) {
        // ignora "." e ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // monta o caminho completo
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

        struct stat pathStat;
        if (stat(fullPath, &pathStat) != 0) {
            fprintf(stderr, RED "Erro ao obter informações do caminho: %s\n" RESET, fullPath);
            continue;
        }

        if (S_ISREG(pathStat.st_mode)) {
            printf(GREEN ">> Processando: %s\n" RESET, fullPath);
            

            if (encryptMode) {
                if (isAlreadyEncrypted(fullPath)) {
                    printf(RED "Ignorado (já criptografado): %s\n" RESET, fullPath);
                    continue;
                }
                if (encrypt(fullPath, keyPath)) {
                    printf(GREEN "Arquivo criptografado com sucesso: %s\n" RESET, fullPath);
                } else {
                    fprintf(stderr, RED "Erro ao criptografar: %s\n" RESET, fullPath);
                }
            } else {
                if (!isAlreadyEncrypted(fullPath)) {
                    printf(RED "Ignorado (não criptografado): %s\n" RESET, fullPath);
                    continue;
                }
                if (decrypt(fullPath, keyPath)) {
                    printf(GREEN "Arquivo descriptografado com sucesso: %s\n" RESET, fullPath);
                } else {
                    fprintf(stderr, RED "Erro ao descriptografar: %s\n" RESET, fullPath);
                }
            }
        }
        else if (S_ISDIR(pathStat.st_mode)) {
            // chamada recursiva para subdiretorios
            processDirectory(fullPath, keyPath, encryptMode);
        }
    }

    closedir(dir);
}
