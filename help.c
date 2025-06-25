#include <stdio.h>
#include "colors.h"
#include "help.h"

// imprime no terminal a lista de comandos suportados pelo programa
void printUsage(const char *programName){
    printf(BLUE "Uso:\n" RESET);
    printf(BLUE "  %s generateKey <arquivo-chave>\n" RESET, programName);
    printf(BLUE "  %s encrypt <arquivo|pasta> <chave>\n" RESET, programName);
    printf(BLUE "  %s decrypt <arquivo|pasta> <chave>\n" RESET, programName);
    printf(BLUE "  %s createFile <arquivo.txt> <chave> <conteúdo>\n" RESET, programName);
    
    printf(BLUE "Exemplos:\n" RESET);
    printf("  %s generateKey minha.key\n", programName);
    printf("  %s createFile segredo.txt minha.key \"minha senha\"\n", programName);
    printf("  %s decrypt segredo.txt minha.key\n", programName);
    printf("  %s encrypt ./meus-arquivos minha.key\n\n", programName);
}
