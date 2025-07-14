#include "heap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int pai(int i) { return (i - 1) / 2; }
int filho_esquerdo(int i) { return 2 * i + 1; }
int filho_direito(int i) { return 2 * i + 2; }

// Lê um elemento do arquivo na posição especificada
void heap_ler_elemento(THeap* heap, int indice, TRegistro* out) {
    if (!heap || !heap->arquivo || !out) {
        if (out) memset(out, 0, sizeof(TRegistro));
        return;
    }
    
    if (indice < 0 || indice >= heap->tamanho) {
        if (out) memset(out, 0, sizeof(TRegistro));
        return;
    }
    
    // Pula o cabeçalho (sizeof(int) para tamanho) e vai para o elemento
    long posicao = sizeof(int) + (long)indice * sizeof(TRegistro);
    
    if (fseek(heap->arquivo, posicao, SEEK_SET) != 0) {
        if (out) memset(out, 0, sizeof(TRegistro));
        return;
    }
    
    if (fread(out, sizeof(TRegistro), 1, heap->arquivo) != 1) {
        if (out) memset(out, 0, sizeof(TRegistro));
        return;
    }
}

// Escreve um elemento no arquivo na posição especificada
void heap_escrever_elemento(THeap* heap, int indice, TRegistro* registro) {
    if (!heap || !heap->arquivo || !registro || indice < 0) {
        return;
    }
    
    // Pula o cabeçalho e vai para o elemento
    long posicao = sizeof(int) + (long)indice * sizeof(TRegistro);
    
    if (fseek(heap->arquivo, posicao, SEEK_SET) != 0) {
        fprintf(stderr, "Erro ao posicionar no arquivo para escrita\n");
        return;
    }
    
    if (fwrite(registro, sizeof(TRegistro), 1, heap->arquivo) != 1) {
        fprintf(stderr, "Erro ao escrever registro no arquivo\n");
        return;
    }
    
    fflush(heap->arquivo);
}

// Atualiza o tamanho no cabeçalho do arquivo
void heap_atualizar_tamanho(THeap* heap) {
    if (!heap || !heap->arquivo) return;
    
    fseek(heap->arquivo, 0, SEEK_SET);
    fwrite(&heap->tamanho, sizeof(int), 1, heap->arquivo);
    fflush(heap->arquivo);
}

// Troca dois elementos no arquivo
void heap_trocar_arquivo(THeap* heap, int i, int j) {
    if (!heap || i == j || i < 0 || j < 0 || i >= heap->tamanho || j >= heap->tamanho) {
        return;
    }
    
    TRegistro temp1, temp2;
    heap_ler_elemento(heap, i, &temp1);
    heap_ler_elemento(heap, j, &temp2);
    
    // Verifica se a leitura foi bem sucedida (CPF não vazio indica registro válido)
    if (strlen(temp1.cpf) == 0 || strlen(temp2.cpf) == 0) {
        return;
    }
    
    heap_escrever_elemento(heap, i, &temp2);
    heap_escrever_elemento(heap, j, &temp1);
}

void heap_heapify_down_arquivo(THeap* heap, int i) {
    if (!heap || i >= heap->tamanho) return;
    
    int maior = i;
    int esq = filho_esquerdo(i);
    int dir = filho_direito(i);
    
    TRegistro elem_i, elem_esq, elem_dir;
    heap_ler_elemento(heap, i, &elem_i);
    
    // Compara com filho esquerdo
    if (esq < heap->tamanho) {
        heap_ler_elemento(heap, esq, &elem_esq);
        if (elem_esq.nota > elem_i.nota) {
            maior = esq;
        }
    }
    
    // Compara com filho direito
    if (dir < heap->tamanho) {
        heap_ler_elemento(heap, dir, &elem_dir);
        if (maior == i) {
            // Se ainda é o pai, compara direto com ele
            if (elem_dir.nota > elem_i.nota) {
                maior = dir;
            }
        } else {
            // Se já mudou para esquerdo, compara com esquerdo
            if (elem_dir.nota > elem_esq.nota) {
                maior = dir;
            }
        }
    }
    
    // Se precisa trocar
    if (maior != i) {
        heap_trocar_arquivo(heap, i, maior);
        heap_heapify_down_arquivo(heap, maior);
    }
}

void heap_heapify_up_arquivo(THeap* heap, int i) {
    // Verificação de segurança
    if (!heap || i <= 0 || i >= heap->tamanho) return;
    
    int p = pai(i);
    TRegistro elem_i, elem_pai;
    
    heap_ler_elemento(heap, i, &elem_i);
    heap_ler_elemento(heap, p, &elem_pai);
    
    if (elem_pai.nota < elem_i.nota) {
        heap_trocar_arquivo(heap, i, p);
        heap_heapify_up_arquivo(heap, p);
    }
}

THeap* heap_inicializa(int capacidade, const char* nomeArquivo) {
    THeap* heap = malloc(sizeof(THeap));
    if (!heap) {
        fprintf(stderr, "Erro ao alocar memória para heap\n");
        return NULL;
    }
    
    heap->capacidade = capacidade;
    heap->tamanho = 0;
    
    // SEMPRE cria um arquivo novo (igual ao hash que sempre reinicializa)
    heap->arquivo = fopen(nomeArquivo, "w+b");
    if (!heap->arquivo) {
        fprintf(stderr, "Erro ao criar arquivo %s\n", nomeArquivo);
        free(heap);
        return NULL;
    }
    
    // Escreve o cabeçalho (tamanho = 0)
    heap_atualizar_tamanho(heap);
    
    return heap;
}

void heap_insere(THeap* heap, TRegistro* registro) {
    if (!heap || !registro || !heap->arquivo) {
        printf("Erro: heap, registro ou arquivo não inicializados\n");
        return;
    }
    
    if (heap->tamanho >= heap->capacidade) {
        printf("Heap cheio! Não é possível inserir mais elementos.\n");
        return;
    }
    
    // Insere o novo elemento na última posição (mas ainda não incrementa tamanho)
    heap_escrever_elemento(heap, heap->tamanho, registro);
    
    // Agora incrementa o tamanho
    heap->tamanho++;
    heap_atualizar_tamanho(heap);
    
    // Restaura a propriedade do heap (agora o índice é tamanho-1)
    if (heap->tamanho > 1) {
        heap_heapify_up_arquivo(heap, heap->tamanho - 1);
    }
}

int heap_busca(THeap* heap, const char* cpf, TRegistro* out) {
    if (!heap || !cpf || !out) return 0;
    
    TRegistro temp;
    for (int i = 0; i < heap->tamanho; i++) {
        heap_ler_elemento(heap, i, &temp);
        if (strcmp(temp.cpf, cpf) == 0) {
            *out = temp;
            return 1;
        }
    }
    return 0;
}

int heap_remove(THeap* heap, const char* cpf) {
    if (!heap || !cpf) return 0;
    
    // Busca o elemento a ser removido
    int indice = -1;
    TRegistro temp;
    for (int i = 0; i < heap->tamanho; i++) {
        heap_ler_elemento(heap, i, &temp);
        if (strcmp(temp.cpf, cpf) == 0) {
            indice = i;
            break;
        }
    }
    
    if (indice == -1) return 0;
    
    // Move o último elemento para a posição do removido
    heap_ler_elemento(heap, heap->tamanho - 1, &temp);
    heap_escrever_elemento(heap, indice, &temp);
    
    heap->tamanho--;
    heap_atualizar_tamanho(heap);
    
    // Restaura a propriedade do heap se necessário
    if (indice < heap->tamanho) {
        heap_heapify_up_arquivo(heap, indice);
        heap_heapify_down_arquivo(heap, indice);
    }
    
    return 1;
}

int heap_extrair_max(THeap* heap, TRegistro* out) {
    if (!heap || heap->tamanho == 0 || !out) return 0;
    
    // Lê o elemento máximo (raiz)
    heap_ler_elemento(heap, 0, out);
    
    // Move o último elemento para a raiz
    TRegistro ultimo;
    heap_ler_elemento(heap, heap->tamanho - 1, &ultimo);
    heap_escrever_elemento(heap, 0, &ultimo);
    
    heap->tamanho--;
    heap_atualizar_tamanho(heap);
    
    // Restaura a propriedade do heap se ainda há elementos
    if (heap->tamanho > 0) {
        heap_heapify_down_arquivo(heap, 0);
    }
    
    return 1;
}

int heap_obter_max(THeap* heap, TRegistro* out) {
    if (!heap || heap->tamanho == 0 || !out) return 0;
    
    heap_ler_elemento(heap, 0, out);
    return 1;
}

void heap_libera(THeap* heap) {
    if (heap) {
        if (heap->arquivo) {
            fclose(heap->arquivo);
        }
        free(heap);
    }
} 