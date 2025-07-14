#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hash.h"
#include "arvorebmais.h"
#include "heap.h"
#include "registro.h"

TRegistro* criarRegistro(const char* cpfStr, const char* nome, int nota) {
    TRegistro* reg = malloc(sizeof(TRegistro));
    strncpy(reg->cpf, cpfStr, 11);
    reg->cpf[11] = '\0';
    strncpy(reg->nome, nome, 50);
    reg->nome[50] = '\0';
    reg->nota = nota;
    return reg;
}

void destruirRegistro(TRegistro* reg) {
    if (reg) {
        free(reg);
    }
}

int carregarRegistros(const char* nomeArquivo, TRegistro** registros, int maxRegistros) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        printf("Erro: arquivo %s não encontrado. Execute primeiro gerarRegistros.\n", nomeArquivo);
        return 0;
    }
    
    int count = 0;
    char linha[200];
    
    while (fgets(linha, sizeof(linha), arquivo) && count < maxRegistros) {
        char nome[51], cpf[12];
        int nota;
        
        char* token = strtok(linha, ",");
        if (!token) continue;
        strncpy(nome, token, 50);
        nome[50] = '\0';
        
        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(cpf, token, 11);
        cpf[11] = '\0';
        
        token = strtok(NULL, ",");
        if (!token) continue;
        nota = atoi(token);
        
        registros[count] = criarRegistro(cpf, nome, nota);
        count++;
    }
    
    fclose(arquivo);
    printf("Carregados %d registros do arquivo %s\n", count, nomeArquivo);
    return count;
}

void testarHash(TRegistro** registros, int numRegistros) {
    printf("\n=== TESTANDO ESTRUTURA HASH ===\n");
    
    THashSecundaria* hash = hash_inicializa(100000);
    if (!hash) {
        printf("Erro ao inicializar hash\n");
        return;
    }
    
    clock_t inicio = clock();
    
    // Inserir primeiros 1000 registros
    printf("Inserindo 1000 registros na hash...\n");
    for (int i = 0; i < 1000 && i < numRegistros; i++) {
        hash_insere(hash, registros[i]);
    }
    
    clock_t fim = clock();
    double tempo_insercao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Tempo de inserção: %.4f segundos\n", tempo_insercao);
    
    // Testar busca
    inicio = clock();
    TRegistro resultado;
    int encontrados = 0;
    for (int i = 0; i < 100 && i < numRegistros; i++) {
        if (hash_busca(hash, registros[i]->cpf, &resultado)) {
            encontrados++;
        }
    }
    fim = clock();
    double tempo_busca = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Busca: %d/100 registros encontrados em %.4f segundos\n", encontrados, tempo_busca);
    
    // Testar remoção
    inicio = clock();
    int removidos = 0;
    for (int i = 0; i < 50 && i < numRegistros; i++) {
        if (hash_remove(hash, registros[i]->cpf)) {
            removidos++;
        }
    }
    fim = clock();
    double tempo_remocao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Remoção: %d registros removidos em %.4f segundos\n", removidos, tempo_remocao);
    
    hash_libera(hash);
}

// Testa operações na Árvore B+
void testarArvoreB(TRegistro** registros, int numRegistros) {
    printf("\n=== TESTANDO ÁRVORE B+ ===\n");
    
    TARVBP* arvore = arvbp_inicializa("dados_arvbmais.bin", "indice_arvbmais.bin");
    if (!arvore) {
        printf("Erro ao inicializar árvore B+\n");
        return;
    }
    
    clock_t inicio = clock();
    
    // Inserir primeiros 1000 registros
    printf("Inserindo 1000 registros na árvore B+...\n");
    for (int i = 0; i < 1000 && i < numRegistros; i++) {
        arvbp_insere(arvore, registros[i]);
    }
    
    clock_t fim = clock();
    double tempo_insercao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Tempo de inserção: %.4f segundos\n", tempo_insercao);
    
    // Testar busca
    inicio = clock();
    TRegistro resultado;
    int encontrados = 0;
    for (int i = 0; i < 100 && i < numRegistros; i++) {
        if (arvbp_busca(arvore, registros[i]->cpf, &resultado)) {
            encontrados++;
        }
    }
    fim = clock();
    double tempo_busca = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Busca: %d/100 registros encontrados em %.4f segundos\n", encontrados, tempo_busca);
    
    // Testar remoção
    inicio = clock();
    int removidos = 0;
    for (int i = 0; i < 50 && i < numRegistros; i++) {
        if (arvbp_remove(arvore, registros[i]->cpf)) {
            removidos++;
        }
    }
    fim = clock();
    double tempo_remocao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Remoção: %d registros removidos em %.4f segundos\n", removidos, tempo_remocao);
    
    arvbp_fecha(arvore, "indice_arvbmais.bin");
}

// Testa operações no Heap
void testarHeap(TRegistro** registros, int numRegistros) {
    printf("\n=== TESTANDO HEAP (PRIORIDADE POR NOTA) ===\n");
    
    THeap* heap = heap_inicializa(10000, "dados_heap.bin");
    if (!heap) {
        printf("Erro ao inicializar heap\n");
        return;
    }
    
    clock_t inicio = clock();
    
    // Inserir primeiros 1000 registros
    printf("Inserindo 1000 registros no heap...\n");
    for (int i = 0; i < 1000 && i < numRegistros; i++) {
        heap_insere(heap, registros[i]);
    }
    
    clock_t fim = clock();
    double tempo_insercao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Tempo de inserção: %.4f segundos\n", tempo_insercao);
    
    // Testar busca
    inicio = clock();
    TRegistro resultado;
    int encontrados = 0;
    for (int i = 0; i < 100 && i < numRegistros; i++) {
        if (heap_busca(heap, registros[i]->cpf, &resultado)) {
            encontrados++;
        }
    }
    fim = clock();
    double tempo_busca = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Busca: %d/100 registros encontrados em %.4f segundos\n", encontrados, tempo_busca);
    
    // Mostrar os 5 maiores (maior prioridade)
    printf("\nTop 5 maiores notas:\n");
    for (int i = 0; i < 5; i++) {
        TRegistro max;
        if (heap_extrair_max(heap, &max)) {
            printf("%d. %s (CPF: %s) - Nota: %d\n", i+1, max.nome, max.cpf, max.nota);
        }
    }
    
    // Testar remoção
    inicio = clock();
    int removidos = 0;
    for (int i = 0; i < 50 && i < numRegistros; i++) {
        if (heap_remove(heap, registros[i]->cpf)) {
            removidos++;
        }
    }
    fim = clock();
    double tempo_remocao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Remoção: %d registros removidos em %.4f segundos\n", removidos, tempo_remocao);
    
    heap_libera(heap);
}

int main() {
    printf("=== SISTEMA DE GERENCIAMENTO DE REGISTROS ===\n");
    printf("Estruturas implementadas: Hash, Árvore B+, Heap\n\n");
    
    // Alocar array para registros
    const int MAX_REGISTROS = 10000;
    TRegistro** registros = malloc(MAX_REGISTROS * sizeof(TRegistro*));
    if (!registros) {
        fprintf(stderr, "Erro ao alocar memória para registros\n");
        return 1;
    }
    
    // Carregar registros do arquivo
    int numRegistros = carregarRegistros("registros.txt", registros, MAX_REGISTROS);
    if (numRegistros == 0) {
        printf("Execute primeiro: gcc gerarRegistros.c registro.h -o gerarRegistros && ./gerarRegistros\n");
        free(registros);
        return 1;
    }
    
    // Testar todas as estruturas
    testarHash(registros, numRegistros);
    testarArvoreB(registros, numRegistros);
    testarHeap(registros, numRegistros);
    
    // Liberar memória
    for (int i = 0; i < numRegistros; i++) {
        destruirRegistro(registros[i]);
    }
    free(registros);
    
    printf("\n=== TESTES CONCLUÍDOS ===\n");
    printf("Arquivos gerados:\n");
    printf("- hash.bin (estrutura hash)\n");
    printf("- dados_arvbmais.bin (dados da árvore B+)\n");
    printf("- dados_heap.bin (dados do heap)\n");
    
    return 0;
}