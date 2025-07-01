#include <stdio.h>
#include <stdlib.h>
#include "hash.c"

int main() {
    long long cpf = 18476737712;
    int cpfTratado = cpf / 100;
    srand(cpfTratado);
    int randomNumber = rand();
    int chave = hash(randomNumber, 255); // hardcoded hash size
    printf("9 primeiros digitos: %d\n", cpfTratado);
    printf("Random number: %d\n", randomNumber);
    printf("Hash key: %d\n", chave);
    return 0;
}