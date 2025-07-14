#include "heap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int pai(int i) { return (i - 1) / 2; }
static int filho_esquerdo(int i) { return 2 * i + 1; }
static int filho_direito(int i) { return 2 * i + 2; }

static void trocar(TRegistro* a, TRegistro* b) {
    TRegistro temp = *a;
    *a = *b;
    *b = temp;
}

static void heapify_down(THeap* heap, int i) {
    int maior = i;
    int esq = filho_esquerdo(i);
    int dir = filho_direito(i);
    
    if (esq < heap->tamanho && heap->elementos[esq].nota > heap->elementos[maior].nota) {
        maior = esq;
    }
    
    if (dir < heap->tamanho && heap->elementos[dir].nota > heap->elementos[maior].nota) {
        maior = dir;
    }
    
    if (maior != i) {
        trocar(&heap->elementos[i], &heap->elementos[maior]);
        heapify_down(heap, maior);
    }
}

static void heapify_up(THeap* heap, int i) {
    while (i > 0 && heap->elementos[pai(i)].nota < heap->elementos[i].nota) {
        trocar(&heap->elementos[i], &heap->elementos[pai(i)]);
        i = pai(i);
    }
}

THeap* heap_inicializa(int capacidade, const char* nomeArquivo) {
    THeap* heap = malloc(sizeof(THeap));
    if (!heap) {
        fprintf(stderr, "Erro ao alocar memória para heap\n");
        return NULL;
    }
    
    heap->elementos = malloc(capacidade * sizeof(TRegistro));
    if (!heap->elementos) {
        fprintf(stderr, "Erro ao alocar memória para elementos do heap\n");
        free(heap);
        return NULL;
    }
    
    heap->tamanho = 0;
    heap->capacidade = capacidade;
    
    heap->arquivoDados = fopen(nomeArquivo, "r+b");
    if (!heap->arquivoDados) {
        heap->arquivoDados = fopen(nomeArquivo, "w+b");
        if (!heap->arquivoDados) {
            fprintf(stderr, "Erro ao criar arquivo %s\n", nomeArquivo);
            free(heap->elementos);
            free(heap);
            return NULL;
        }
    }
    
    return heap;
}

void heap_insere(THeap* heap, TRegistro* registro) {
    if (!heap || !registro) return;
    
    if (heap->tamanho >= heap->capacidade) {
        fprintf(stderr, "Heap cheio! Não é possível inserir mais elementos.\n");
        return;
    }
    
    heap->elementos[heap->tamanho] = *registro;
    heap->tamanho++;
    
    heapify_up(heap, heap->tamanho - 1);
    
    heap_salva(heap);
}

int heap_busca(THeap* heap, const char* cpf, TRegistro* out) {
    if (!heap || !cpf || !out) return 0;
    
    for (int i = 0; i < heap->tamanho; i++) {
        if (strcmp(heap->elementos[i].cpf, cpf) == 0) {
            *out = heap->elementos[i];
            return 1;
        }
    }
    return 0;
}

int heap_remove(THeap* heap, const char* cpf) {
    if (!heap || !cpf) return 0;
    
    int indice = -1;
    for (int i = 0; i < heap->tamanho; i++) {
        if (strcmp(heap->elementos[i].cpf, cpf) == 0) {
            indice = i;
            break;
        }
    }
    
    if (indice == -1) return 0;
    
    heap->elementos[indice] = heap->elementos[heap->tamanho - 1];
    heap->tamanho--;
    
    if (indice < heap->tamanho) {
        heapify_up(heap, indice);
        heapify_down(heap, indice);
    }
    
    heap_salva(heap);
    return 1;
}

int heap_extrair_max(THeap* heap, TRegistro* out) {
    if (!heap || heap->tamanho == 0 || !out) return 0;
    
    *out = heap->elementos[0];
    
    heap->elementos[0] = heap->elementos[heap->tamanho - 1];
    heap->tamanho--;
    
    if (heap->tamanho > 0) {
        heapify_down(heap, 0);
    }
    
    heap_salva(heap);
    return 1;
}

int heap_obter_max(THeap* heap, TRegistro* out) {
    if (!heap || heap->tamanho == 0 || !out) return 0;
    
    *out = heap->elementos[0];
    return 1;
}

void heap_salva(THeap* heap) {
    if (!heap || !heap->arquivoDados) return;
    
    rewind(heap->arquivoDados);
    
    fwrite(&heap->tamanho, sizeof(int), 1, heap->arquivoDados);
    
    fwrite(heap->elementos, sizeof(TRegistro), heap->tamanho, heap->arquivoDados);
    fflush(heap->arquivoDados);
}

THeap* heap_carrega(const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo) return NULL;
    
    int tamanho;
    if (fread(&tamanho, sizeof(int), 1, arquivo) != 1) {
        fclose(arquivo);
        return NULL;
    }
    
    int capacidade = tamanho > 100 ? tamanho * 2 : 100;
    THeap* heap = heap_inicializa(capacidade, nomeArquivo);
    if (!heap) {
        fclose(arquivo);
        return NULL;
    }
    
    heap->tamanho = tamanho;
    fread(heap->elementos, sizeof(TRegistro), tamanho, arquivo);
    fclose(arquivo);
    
    return heap;
}

void heap_libera(THeap* heap) {
    if (heap) {
        if (heap->arquivoDados) {
            heap_salva(heap);
            fclose(heap->arquivoDados);
        }
        if (heap->elementos) {
            free(heap->elementos);
        }
        free(heap);
    }
} 