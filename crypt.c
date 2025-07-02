#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "crypt.h"
#include "colors.h"

// terminal candy: simple progress bar
void showProgressBar(int percent) {
    int barWidth = 50;
    int pos = (percent * barWidth) / 100;

    printf("\r["); // \r volta pro começo da linha
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos)
            printf("█");
        else
            printf(" ");
    }
    printf("] %d%%", percent);
    fflush(stdout); // força o terminal a atualizar a linha
}

// gera uma nova chave e salva
void generateKey(const char* filePath){
    const char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    char key[KEY_SIZE + 1];

    // abrir para escrita, cria se nao existir
    FILE* file = fopen(filePath, "w");
    if (!file) {
        perror("Erro ao abrir arquivo");
        return;
    }

    // gera a parte aleatoia da chave
    srand(time(NULL));
    for (int i = 0; i < KEY_SIZE; i++) {
        int index = rand() % (sizeof(characters) - 1);
        key[i] = characters[index];
    }
    key[KEY_SIZE] = '\0';

    // escreve no arquivo: HEADER + key
    fputs(HEADER_KEY, file);
    fputs(key, file);
    fclose(file);

    printf(GREEN "Chave gerada e salva com sucesso!\n" RESET);
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
        fprintf(stderr, RED "Chave inválida: header incorreto\n" RESET);
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

    printf("Criptografando: ");
    int lastPercent = -1;

    // criptografa os dados usando soma modular com a chave
    for (long i = 0; i < fileSize; i++) {
        buffer[i] = (buffer[i] + key[i % keySize]) % 256;

        // barra de progresso
        int percent = (i * 100) / fileSize;
        if (percent != lastPercent) {
            showProgressBar(percent);
            lastPercent = percent;
        }
    }
    printf("\n");

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

// descriptografa arquivo com a chave carregada
int decrypt(const char* filePath, const char* keyPath){
    // abre o arquivo
    FILE* file = fopen(filePath, "rb");
    if(!file){
        perror("Erro ao abrir o arquivo");
        return 0;
    }

    // abre o arquivo da chave
    FILE* keyFile = fopen(keyPath, "rb");
    if(!keyFile){
        perror("Erro ao abrir o arquivo da chave");
        fclose(file);
        return 0;
    }

    // le e verifica o header "CRYPTED"
    char headerBuffer[HEADER_CRYPTO_SIZE];
    fread(headerBuffer, 1, HEADER_CRYPTO_SIZE, file);

    if (memcmp(headerBuffer, HEADER_CRYPTO, HEADER_CRYPTO_SIZE) != 0) {
        fprintf(stderr, "Erro: Arquivo não está criptografado ou está corrompido.\n");
        fclose(file);
        fclose(keyFile);
        return 0;
    }

    // pega o tamanho do arquivo
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    // calcula o tamanho dos dados reais obs: sem o header
    long dataSize = fileSize - HEADER_CRYPTO_SIZE;

    // aloca memoria
    unsigned char* buffer = malloc(dataSize);
    fseek(file, HEADER_CRYPTO_SIZE, SEEK_SET); // pula o header
    fread(buffer, 1, dataSize, file);
    fclose(file);

    // lê a chave
    fseek(keyFile, 0, SEEK_END);
    long keySize = ftell(keyFile);
    rewind(keyFile);

    unsigned char* key = malloc(keySize);
    fread(key, 1, keySize, keyFile);
    fclose(keyFile);

    if (!buffer || !key) {
        perror("Erro de memória");
        free(buffer);
        free(key);
        return 0;
    }

    printf("Descriptografando: ");
    int lastPercent = -1;

    // descriptografa
    for (long i = 0; i < dataSize; i++) {
        buffer[i] = (256 + buffer[i] - key[i % keySize]) % 256;

        // barra de progresso
        int percent = (i * 100) / dataSize;
        if (percent != lastPercent) {
            showProgressBar(percent);
            lastPercent = percent;
        }
    }
    printf("\n");

    // sobrescreve o arquivo com os dados restaurados
    FILE* outFile = fopen(filePath, "wb");
    if (!outFile) {
        perror("Erro ao abrir arquivo para escrita");
        free(buffer);
        free(key);
        return 0;
    }

    fwrite(buffer, 1, dataSize, outFile);
    fclose(outFile);

    //libera a memoria
    free(buffer);
    free(key);
    return 1;
}

// verifica se o arquivo ja esta criptografado ao checar o header "CRYPTED" no inicio
int isAlreadyEncrypted(const char* filePath) {
    FILE* file = fopen(filePath, "rb");
    if (!file) return 0;

    char header[HEADER_CRYPTO_SIZE];
    size_t readBytes = fread(header, 1, HEADER_CRYPTO_SIZE, file);
    fclose(file);

    return (readBytes == HEADER_CRYPTO_SIZE && memcmp(header, HEADER_CRYPTO, HEADER_CRYPTO_SIZE) == 0);
}

// cria um novo arquivo com o conteúdo fornecido e o criptografa
void create_and_encrypt_file(const char* filename, const char* keyfile, const char* content) {
    FILE* f = fopen(filename, "rb");
    if (f) {
        fclose(f);
        fprintf(stderr, RED "Erro: o arquivo '%s' já existe.\n" RESET, filename);
        return;
    }

    // valida extensão .txt
    const char* ext = strrchr(filename, '.');
    if (!ext || strcmp(ext, ".txt") != 0) {
        fprintf(stderr, RED "Erro: o arquivo deve ter extensão .txt\n" RESET);
        return;
    }

    f = fopen(filename, "wb");
    if (!f) {
        printf(RED "Erro ao criar o arquivo.\n" RESET);
        return;
    }
    fwrite(content, sizeof(char), strlen(content), f);
    fclose(f);

    encrypt(filename, keyfile);
}