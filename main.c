#include <stdio.h>
#include <string.h>
#include "crypt.h"

int main() {
    unsigned char key[KEY_SIZE];

    const char* keyPath = "chave.key";

    if (loadKey(keyPath, key, KEY_SIZE)) {
        printf("Chave carregada com sucesso!\n");
        for (int i = 0; i < 16; i++) {
            printf("%02X ", key[i]);
        }
        printf("\n");
    } else {
        fprintf(stderr, "Falha ao carregar a chave.\n");
    }

    return 0;
}
