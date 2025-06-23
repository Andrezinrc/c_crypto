#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "crypt.h"

// gera uma nova chave e salva no caminho informado
void GenerateKey(){
    char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    int keySize = strlen(characters);
    int KeyLength = 32; 

    // abrir para escrita, cria se não existir
    FILE* file = fopen("chave.key", "w");
    if (!file) {
        perror("Erro ao abrir arquivo");
        return;
    }
    
    for (int i = 0; i < KeyLength; i++) {
        int index = rand() % keySize;
        fputc(characters[index], file);  // escreve caractere no arquivo
    }

    fclose(file);
    printf("Chave gerada e salva em chave.key\n");
}