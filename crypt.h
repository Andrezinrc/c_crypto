#ifndef CRYPT_H
#define CRYPT_H

#include <stddef.h>

#define KEY_SIZE 32
#define HEADER_KEY "CRYPT::KEY"
#define HEADER_KEY_SIZE 10

// gera uma nova chave e salva
// carrega chave de um arquivo
// criptografa arquivos com a chave carregada

void generateKey();
int loadKey(const char* path, unsigned char* keyBuffer, size_t bufferSize);
#endif
