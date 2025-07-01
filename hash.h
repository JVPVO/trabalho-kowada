#ifndef HASH_H
#define HASH_H

#include "aluno.h"

typedef struct tipohashSecundaria {
    char *nomeArquivo;
    int tamanho;
} hashSecundaria;

hashSecundaria* inicializa(int tamanho);
int hashar(int valor, int tamanho);
void inserir(hashSecundaria* hash, Taluno* aluno);

#endif
