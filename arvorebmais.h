#ifndef ARVOREBMAIS_H
#define ARVOREBMAIS_H

#include <stdio.h>
#include "registro.h"

#define ORDEM_BP 3   // Ordem mínima t=3 (máx 2*t chaves por nó)

typedef struct no_arvbmais {
    int folha;                   // 1 se folha
    int n_chaves;                // quantidade de chaves
    int chaves[2*ORDEM_BP];      // 9 primeiros dígitos do CPF
    long deslocs[2*ORDEM_BP];    // Apenas em nós folha: offset do registro em dados_arvbmais.bin
    struct no_arvbmais* filhos[2*ORDEM_BP+1]; // ponteiros para filhos
    struct no_arvbmais* prox;    // ponteiro para próxima folha
} TNoArvBMais;

typedef struct arvbmais {
    TNoArvBMais* raiz;
    FILE* arquivoDados;   // dados_arvbmais.bin
} TARVBP;

TARVBP* arvbp_inicializa(const char* nome_dados, const char* nome_indice);

void arvbp_fecha(TARVBP* arvore, const char* nome_indice);

void arvbp_insere(TARVBP* arvore, TRegistro* registro);

int arvbp_busca(TARVBP* arvore, const char* cpf, TRegistro* out);

int arvbp_remove(TARVBP* arvore, const char* cpf);

#endif // ARVOREBMAIS_H 