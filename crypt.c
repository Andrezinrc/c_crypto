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

// criptografa arquivos com a chave carregada
int encrypt(const char* filePath, const char* keyPath) {
    FILE* file = fopen(filePath, "rb");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        return 0;
    }

    // abre o arquivo da chave
    FILE* keyFile = fopen(keyPath, "rb");
    if (!keyFile) {
        perror("Erro ao abrir o arquivo da chave");
        fclose(file);
        return 0;
    }

    // pega o tamanho do arquivo
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    // pega o tamanho da chave
    fseek(keyFile, 0, SEEK_END);
    long keySize = ftell(keyFile);
    rewind(keyFile);

    // aloca memoria para dados e chave
    unsigned char* buffer = malloc(fileSize);
    unsigned char* key = malloc(keySize);
    if (!buffer || !key) {
        perror("Erro ao alocar memória");
        fclose(file);
        fclose(keyFile);
        free(buffer);
        free(key);
        return 0;
    }

    // le os dados do arquivo e da chave
    fread(buffer, 1, fileSize, file);
    fread(key, 1, keySize, keyFile);
    fclose(file);
    fclose(keyFile);

    // criptografa os dados usando soma modular com a chave
    for (long i = 0; i < fileSize; i++) {
        buffer[i] = (buffer[i] + key[i % keySize]) % 256;
    }

    // reabre o arquivo para sobrescrever com os dados criptografados
    FILE* outFile = fopen(filePath, "wb");
    if (!outFile) {
        perror("Erro ao abrir arquivo para escrita");
        free(buffer);
        free(key);
        return 0;
    }

    // escreve o header "CRYPTED" no início do arquivo para identificacao
    fwrite(HEADER_CRYPTO, 1, HEADER_CRYPTO_SIZE, outFile);

    // escreve os dados criptografados apos o header
    fwrite(buffer, 1, fileSize, outFile);
    fclose(outFile);

    // libera memoria e retorna
    free(buffer);
    free(key);

    return 1;
}
