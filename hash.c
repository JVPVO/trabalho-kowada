#include <stdio.h>
#include <stdlib.h>
#include "aluno.h"

typedef struct tipohashSecundaria {
    char *nomeArquivo;
    int tamanho; // 100.000 
} hashSecundaria;

hashSecundaria* inicializa(int tamanho){
    FILE* arquivo = fopen("hash.bin", "rb");
    if(!arquivo) fopen("hash.bin", "w");
    fclose(arquivo);
    hashSecundaria* hash = malloc(sizeof(hashSecundaria));
    hash->nomeArquivo = "hash.bin";
    hash->tamanho = 100000; // 100.000 
    return hash; 
}

void inserir(hashSecundaria* hash, Taluno* aluno){
    int cpfTratado = (aluno->CPF)/100;
    int reg = hashar(cpfTratado, hash->tamanho);
    FILE* arquivo = fopen(hash->nomeArquivo, "wb");
    fseek(arquivo, (reg-1)*sizeof(Taluno), SEEK_SET);
    return;
}

int hashar(int valor, int tamanho){
    return valor % tamanho;
}