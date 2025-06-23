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

void generateKey();
int loadKey(const char* path, unsigned char* keyBuffer, size_t bufferSize);
int encrypt(const char* filePath, const char* keyPath);

#endif
