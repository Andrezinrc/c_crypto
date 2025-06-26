#include <stdio.h>
#include "colors.h"
#include "help.h"

// imprime no terminal a lista de comandos suportados pelo programa
void printUsage(const char *programName){
    printf(BLUE "Uso:\n" RESET);
    printf("  %s generateKey <arquivo-chave>\n", programName);
    printf("  %s encrypt <arquivo|pasta> <chave>\n", programName);
    printf("  %s decrypt <arquivo|pasta> <chave>\n", programName);
    printf("  %s createFile <arquivo.txt> <chave> <conteúdo>\n", programName);
    
    printf(BLUE "Exemplos:\n" RESET);
    printf("  %s generateKey minha.key\n", programName);
    printf("  %s createFile segredo.txt minha.key \"minha senha\"\n", programName);
    printf("  %s decrypt segredo.txt minha.key\n", programName);
    printf("  %s encrypt ./meus-arquivos minha.key\n\n", programName);
}
