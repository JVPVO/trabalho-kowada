#include <stdio.h>
#include <stdlib.h>
#include "hash.h"

hashSecundaria* inicializa(int tamanho){
    FILE* arquivo = fopen("hash.bin", "rb");
    if(!arquivo) fopen("hash.bin", "w");
    fclose(arquivo);
    hashSecundaria* hash = malloc(sizeof(hashSecundaria));
    hash->nomeArquivo = "hash.bin";
    hash->tamanho = 100000; // 100.000 
    return hash; 
}

int hashar(int valor, int tamanho){
    return valor % tamanho;
}

void inserir(hashSecundaria* hash, Taluno* aluno){
    int cpfTratado = (aluno->CPF)/100;
    int reg = hashar(cpfTratado, hash->tamanho);
    FILE* arquivo = fopen(hash->nomeArquivo, "wb");
    fseek(arquivo, (reg-1)*sizeof(Taluno), SEEK_SET);
    fwrite(aluno, sizeof(Taluno), 1, arquivo);
    fclose(arquivo);
    return;
}
