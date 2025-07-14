#ifndef HASH_H
#define HASH_H

#include "registro.h"

typedef struct hashSecundaria {
    FILE* arquivo;
    int tamanho;
} THashSecundaria;

THashSecundaria* hash_inicializa(int tamanho);

int hashar(int valor, int tamanho);

void hash_insere(THashSecundaria* hash, TRegistro* registro);

int hash_busca(THashSecundaria* hash, const char* cpf, TRegistro* out);

int hash_remove(THashSecundaria* hash, const char* cpf);

void hash_libera(THashSecundaria* hash);

// Funções para controle do contador de colisões
void hash_resetar_colisoes();
int hash_obter_colisoes();

#endif
