#ifndef ARVOREBMAIS_H
#define ARVOREBMAIS_H

#include <stdio.h>
#include "registro.h"

#define ORDEM_BP 3

typedef struct no_arvbmais {
    int folha;
    int n_chaves;
    int chaves[2*ORDEM_BP];
    long deslocs[2*ORDEM_BP];
    struct no_arvbmais* filhos[2*ORDEM_BP+1];
    struct no_arvbmais* prox;
} TNoArvBMais;

typedef struct arvbmais {
    TNoArvBMais* raiz;
    FILE* arquivoDados;
} TARVBP;

TARVBP* arvbp_inicializa(const char* nome_dados, const char* nome_indice);

void arvbp_fecha(TARVBP* arvore, const char* nome_indice);

void arvbp_insere(TARVBP* arvore, TRegistro* registro);

int arvbp_busca(TARVBP* arvore, const char* cpf, TRegistro* out);

int arvbp_remove(TARVBP* arvore, const char* cpf);

#endif 