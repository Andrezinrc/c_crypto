#ifndef CRYPT_H
#define CRYPT_H

#include <stddef.h>

//HEADER CHAVE

#define KEY_SIZE 32
#define HEADER_KEY "CRYPT::KEY"
#define HEADER_KEY_SIZE 10

// HEADER CRIPTOGRAFIA

#define HEADER_CRYPTO "CRYPTED"
#define HEADER_CRYPTO_SIZE 7


// gera uma nova chave e salva
// carrega chave de um arquivo
// criptografa arquivos com a chave carregada
// descriptografa arquivo com a chave carregada
// verifica se o arquivo ja esta criptografado ao checar o header "CRYPTED" no inicio
// cria um novo arquivo com o conteúdo fornecido e o criptografa

void generateKey(const char* filePath);
int loadKey(const char* path, unsigned char* keyBuffer, size_t bufferSize);
int encrypt(const char* filePath, const char* keyPath);
int decrypt(const char* filePath, const char* keyPath);
int isAlreadyEncrypted(const char* filePath);
void create_and_encrypt_file(const char* filename, const char* keyfile, const char* content);

#endif
