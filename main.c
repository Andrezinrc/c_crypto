#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "crypt.h"

int main(){
    srand(time(NULL));

    GenerateKey();
    return 0;
}