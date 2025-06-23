#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "crypt.h"

// gera uma nova chave e salva
void generateKey(){
    const char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    char key[HEADER_KEY_SIZE + 1];
        
    // abrir para escrita, cria se nao existir
    FILE* file = fopen("chave.key", "w");
    if (!file) {
        perror("Erro ao abrir arquivo");
        return;
    }
    
    // gera a parte aleatoia da chave
    srand(time(NULL));
    for (int i = 0; i < HEADER_KEY_SIZE; i++) {
        int index = rand() % (sizeof(characters) - 1);
        key[i] = characters[index];
    }
    key[HEADER_KEY_SIZE] = '\0';

    // escreve no arquivo: HEADER + key
    fputs(HEADER_KEY, file);
    fputs(key, file);
    fclose(file);

    printf("Chave gerada e salva com sucesso!\n");
}

// carrega chave de um arquivo
int loadKey(const char* path, unsigned char* keyBuffer, size_t bufferSize) {
    // garante que o buffer fornecido é grande o suficiente para armazenar a chave
    if (bufferSize < KEY_SIZE) {
        fprintf(stderr, "Buffer pequeno demais para armazenar a chave.\n");
        return 0;
    }

    FILE* f = fopen(path, "rb");
    if (!f) {
        perror("Erro ao abrir o arquivo da chave");
        return 0;
    }

    // le e valida o header do arquivo de chave
    char fileHeader[HEADER_KEY_SIZE + 1] = {0}; // +1 para '\0'
    size_t readHeader = fread(fileHeader, sizeof(char), HEADER_KEY_SIZE, f);
    if (readHeader != HEADER_KEY_SIZE || strcmp(fileHeader, HEADER_KEY) != 0) {
        fclose(f);
        fprintf(stderr, "Chave inválida: header incorreto\n");
        return 0;
    }

    // le os bytes da chave apos o header
    size_t readKey = fread(keyBuffer, sizeof(char), KEY_SIZE, f);
    fclose(f);

    // verifica se todos os bytes esperados foram lidos
    if (readKey < KEY_SIZE) {
        fprintf(stderr, "Chave inválida: tamanho insuficiente\n");
        return 0;
    }

    return 1;
}
