#ifndef HEAP_H
#define HEAP_H

#include <stdio.h>
#include "registro.h"

typedef struct heap {
    TRegistro* elementos;     // Array de registros
    int tamanho;             // Número atual de elementos
    int capacidade;          // Capacidade máxima
    FILE* arquivoDados;      // Arquivo de dados do heap
} THeap;

THeap* heap_inicializa(int capacidade, const char* nomeArquivo);

void heap_insere(THeap* heap, TRegistro* registro);

int heap_busca(THeap* heap, const char* cpf, TRegistro* out);

int heap_remove(THeap* heap, const char* cpf);

int heap_extrair_max(THeap* heap, TRegistro* out);

int heap_obter_max(THeap* heap, TRegistro* out);

void heap_libera(THeap* heap);

THeap* heap_carrega(const char* nomeArquivo);

void heap_salva(THeap* heap);

#endif // HEAP_H 