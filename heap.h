#ifndef HEAP_H
#define HEAP_H

#include <stdio.h>
#include "registro.h"

typedef struct heap {
    FILE* arquivo;           // Arquivo de dados do heap
    int tamanho;            // Número atual de elementos no arquivo
    int capacidade;         // Capacidade máxima
} THeap;

THeap* heap_inicializa(int capacidade, const char* nomeArquivo);

void heap_insere(THeap* heap, TRegistro* registro);

int heap_busca(THeap* heap, const char* cpf, TRegistro* out);

int heap_remove(THeap* heap, const char* cpf);

int heap_extrair_max(THeap* heap, TRegistro* out);

int heap_obter_max(THeap* heap, TRegistro* out);

void heap_libera(THeap* heap);

// Funções auxiliares para trabalhar com arquivo
void heap_ler_elemento(THeap* heap, int indice, TRegistro* out);
void heap_escrever_elemento(THeap* heap, int indice, TRegistro* registro);
void heap_heapify_down_arquivo(THeap* heap, int i);
void heap_heapify_up_arquivo(THeap* heap, int i);

#endif // HEAP_H 