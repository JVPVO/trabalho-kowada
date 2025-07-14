#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hash.h"
#include "arvorebmais.h"
#include "heap.h"
#include "registro.h"
#include <locale.h>

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
    
    THashSecundaria* hash = hash_inicializa(100019);
    if (!hash) {
        printf("Erro ao inicializar hash\n");
        return;
    }
    
    hash_resetar_colisoes();
    clock_t inicio = clock();
    
    printf("Inserindo %d registros na hash...\n", numRegistros);
    for (int i = 0; i < numRegistros; i++) {
        hash_insere(hash, registros[i]);
    }
    
    clock_t fim = clock();
    double tempo_insercao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Tempo de inserção: %.4f segundos\n", tempo_insercao);
    
    printf("Colisões durante inserção: %d\n", hash_obter_colisoes());
    
    inicio = clock();
    TRegistro resultado;
    int encontrados = 0;
    for (int i = 0; i < numRegistros; i++) {
        if (hash_busca(hash, registros[i]->cpf, &resultado)) {
            encontrados++;
        }
    }
    fim = clock();
    double tempo_busca = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Busca: %d/%d registros encontrados em %.4f segundos\n", encontrados, numRegistros, tempo_busca);
    
    /*
    inicio = clock();
    int removidos = 0;
    for (int i = 0; i < numRegistros; i++) {
        if (hash_remove(hash, registros[i]->cpf)) {
            removidos++;
        }
    }
    fim = clock();
    double tempo_remocao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Remoção: %d registros removidos em %.4f segundos\n", removidos, tempo_remocao);
    */
    
    hash_libera(hash);
}

void testarArvoreB(TRegistro** registros, int numRegistros) {
    printf("\n=== TESTANDO ÁRVORE B+ ===\n");
    
    TARVBP* arvore = arvbp_inicializa("dados_arvbmais.bin", "indice_arvbmais.bin");
    if (!arvore) {
        printf("Erro ao inicializar árvore B+\n");
        return;
    }
    
    clock_t inicio = clock();
    
    printf("Inserindo %d registros na árvore B+...\n", numRegistros);
    for (int i = 0; i < numRegistros; i++) {
        arvbp_insere(arvore, registros[i]);
    }
    
    clock_t fim = clock();
    double tempo_insercao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Tempo de inserção: %.4f segundos\n", tempo_insercao);
    
    inicio = clock();
    TRegistro resultado;
    int encontrados = 0;
    for (int i = 0; i < numRegistros; i++) {
        if (arvbp_busca(arvore, registros[i]->cpf, &resultado)) {
            encontrados++;
        }
    }
    fim = clock();
    double tempo_busca = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Busca: %d/%d registros encontrados em %.4f segundos\n", encontrados, numRegistros, tempo_busca);
    
    /*
    inicio = clock();
    int removidos = 0;
    
    for (int i = 0; i < numRegistros; i++) {
        if (arvbp_remove(arvore, registros[i]->cpf)) {
            removidos++;
        }
    }
    
    fim = clock();
    double tempo_remocao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Remoção: %d registros removidos em %.4f segundos\n", removidos, tempo_remocao);
    */
    
    arvbp_fecha(arvore, "indice_arvbmais.bin");
}

void testarHeap(TRegistro** registros, int numRegistros) {
    printf("\n=== TESTANDO HEAP (PRIORIDADE POR NOTA) ===\n");
    
    FILE* f = fopen("dados_heap.bin", "w");
    if (f) fclose(f);

    THeap* heap = heap_inicializa(numRegistros, "dados_heap.bin");
    if (!heap) {
        printf("Erro ao inicializar heap\n");
        return;
    }
    
    clock_t inicio = clock();
    
    printf("Inserindo %d registros no heap...\n", numRegistros);
    for (int i = 0; i < numRegistros; i++) {
        heap_insere(heap, registros[i]);
    }
    
    clock_t fim = clock();
    double tempo_insercao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
    printf("Tempo de inserção: %.4f segundos\n", tempo_insercao);
    
    TRegistro resultado;
    if (numRegistros > 0) {
        printf("\nBuscando um registro específico (o primeiro da lista)...\n");
        if (heap_busca(heap, registros[0]->cpf, &resultado)) {
            printf("Registro encontrado: %s, Nota: %d\n", resultado.nome, resultado.nota);
        } else {
            printf("Registro não encontrado.\n");
        }
    }

    printf("\nTop 5 maiores notas (extraindo do heap):\n");
    for (int i = 0; i < 5 && heap->tamanho > 0; i++) {
        TRegistro max;
        if (heap_extrair_max(heap, &max)) {
            printf("%d. %s (CPF: %s) - Nota: %d\n", i+1, max.nome, max.cpf, max.nota);
        }
    }
    
    printf("\nTeste de remoção em massa não executado para heap (evitar lentidão de O(N^2)).\n");
    
    heap_libera(heap);
}

int main() {
    setlocale(LC_NUMERIC, "pt_BR.UTF-8");
    printf("=== SISTEMA DE GERENCIAMENTO DE REGISTROS ===\n");
    printf("Estruturas implementadas: Hash, Árvore B+, Heap\n\n");
    
    int MAX_REGISTROS = 10000;
    TRegistro** registros = malloc(MAX_REGISTROS * sizeof(TRegistro*));
    if (!registros) {
        printf("Erro ao alocar memória para registros\n");
        return 1;
    }
    
    int numRegistros = carregarRegistros("registros.txt", registros, MAX_REGISTROS);
    if (numRegistros == 0) {
        printf("Execute primeiro: gcc gerarRegistros.c registro.h -o gerarRegistros && ./gerarRegistros\n");
        free(registros);
        return 1;
    }
    
    testarHash(registros, numRegistros);
    testarArvoreB(registros, numRegistros);
    testarHeap(registros, numRegistros);
    
    for (int i = 0; i < numRegistros; i++) {
        destruirRegistro(registros[i]);
    }
    free(registros);
    
    printf("\n=== TESTES CONCLUÍDOS ===\n");
    printf("Arquivos gerados:\n");
    printf("- hash.bin (estrutura hash)\n");
    printf("- dados_arvbmais.bin (dados da árvore B+)\n");
    printf("- dados_heap.bin (dados do heap)\n");
    

    //MINI MENU
    int ED = 0;
    // 1- Hash 2- Arvore B+ 3- Heap
    while (ED != 4){
        printf("Menu, cada estrutura de dados tem um menu interno e uma 'cópia' dos registros:\n");
        printf("1 - Hash\n");
        printf("2 - Arvore B+\n");
        printf("3 - Heap\n");
        printf("4 - Sair\n");
        scanf("%d", &ED);
        switch (ED){
            case 1:
                THashSecundaria* hash = hash_carrega_existente(100019);
                if (!hash) {
                    printf("Erro ao abrir arquivo hash.bin, os arquivos .bin foram apagados após os testes iniciais?\n");
                    break;
                }
                int opcao = 0;
                while (opcao != 4){
                    printf("1 - Buscar registro\n");
                    printf("2 - Remover registro\n");
                    printf("3 - Listar todos os registros\n");
                    printf("4 - Sair do menu\n");
                    scanf("%d", &opcao);
                    switch (opcao){
                       case 1:
                           printf("Digite o CPF do registro: ");
                           char cpf[12];
                           scanf("%s", cpf);
                           TRegistro registro;
                           if (hash_busca(hash, cpf, &registro)) {
                               printf("Registro encontrado: %s, Nota: %d\n", registro.nome, registro.nota);
                           } else {
                               printf("Registro não encontrado.\n");
                           }
                           break;
                       case 2:
                           printf("Digite o CPF do registro: ");
                           scanf("%s", cpf);
                           if (hash_remove(hash, cpf)) {
                               printf("Registro removido com sucesso.\n");
                           } else {
                               printf("Registro não encontrado.\n");
                           }
                           break;
                       case 3:
                           printf("Listando todos os registros na hash:\n");
                           fseek(hash->arquivo, 0, SEEK_SET);
                           for (int i = 0; i < hash->tamanho; i++) {
                               TRegistro reg;
                               if (fread(&reg, sizeof(TRegistro), 1, hash->arquivo) == 1) {
                                   if (strlen(reg.cpf) > 0) {
                                       printf("%s, %s, %d\n", reg.nome, reg.cpf, reg.nota);
                                   }
                               }
                           }
                           break;
                       case 4:
                           printf("Saindo do menu...\n");
                           hash_libera(hash);
                           break;
                   }
                }
                break;
             case 2:
                 TARVBP* arvore = arvbp_carrega_existente("dados_arvbmais.bin", "indice_arvbmais.bin");
                 if (!arvore) {
                     printf("Erro ao abrir arquivo dados_arvbmais.bin, os arquivos .bin foram apagados após os testes iniciais?\n");
                     break;
                 }
                 int opcao2 = 0;
                 while (opcao2 != 4){
                     printf("1 - Buscar registro\n");
                     printf("2 - Remover registro\n");
                     printf("3 - Listar todos os registros\n");
                     printf("4 - Sair do menu\n");
                     scanf("%d", &opcao2);
                     switch (opcao2){
                         case 1:
                             printf("Digite o CPF do registro: ");
                             char cpf2[12];
                             scanf("%s", cpf2);
                             TRegistro registro2;
                             if (arvbp_busca(arvore, cpf2, &registro2)) {
                                 printf("Registro encontrado: %s, Nota: %d\n", registro2.nome, registro2.nota);
                             } else {
                                 printf("Registro não encontrado.\n");
                             }
                             break;
                         case 2:
                             printf("Digite o CPF do registro: ");
                             scanf("%s", cpf2);
                             if (arvbp_remove(arvore, cpf2)) {
                                 printf("Registro removido com sucesso.\n");
                             } else {
                                 printf("Registro não encontrado.\n");
                             }
                             break;
                         case 3:
                             printf("Listando todos os registros...\n");
                             TRegistro reg;
                             fseek(arvore->arquivoDados, 0, SEEK_SET);
                             while (fread(&reg, sizeof(TRegistro), 1, arvore->arquivoDados) == 1) {
                                 if (strlen(reg.cpf) > 0) {
                                     printf("%s, %s, %d\n", reg.nome, reg.cpf, reg.nota);
                                 }
                             }
                             break;
                         case 4:
                             printf("Saindo do menu...\n");
                             fclose(arvore->arquivoDados);
                             free(arvore);
                             break;
                     }
                 }
                 break;
             case 3:
                 THeap* heap = heap_carrega_existente("dados_heap.bin");
                 if (!heap) {
                     printf("Erro ao abrir arquivo dados_heap.bin, os arquivos .bin foram apagados após os testes iniciais?\n");
                     break;
                 }
                 int opcao3 = 0;
                 while (opcao3 != 5){
                     printf("1 - Buscar registro\n");
                     printf("2 - Remover registro\n");
                     printf("3 - Obter registro com maior nota\n");
                     printf("4 - Listar todos os registros\n");
                     printf("5 - Sair do menu\n");
                     scanf("%d", &opcao3);
                     switch (opcao3){
                         case 1:
                             printf("Digite o CPF do registro: ");
                             char cpf3[12];
                             scanf("%s", cpf3);
                             TRegistro registro3;
                             if (heap_busca(heap, cpf3, &registro3)) {
                                 printf("Registro encontrado: %s, Nota: %d\n", registro3.nome, registro3.nota);
                             } else {
                                 printf("Registro não encontrado.\n");
                             }
                             break;
                         case 2:
                             printf("Digite o CPF do registro: ");
                             scanf("%s", cpf3);
                             if (heap_remove(heap, cpf3)) {
                                 printf("Registro removido com sucesso.\n");
                             } else {
                                 printf("Registro não encontrado.\n");
                             }
                             break;
                         case 3:
                             TRegistro max;
                             if (heap_obter_max(heap, &max)) {
                                 printf("Registro com maior nota: %s, Nota: %d\n", max.nome, max.nota);
                             } else {
                                 printf("Heap está vazio.\n");
                             }
                             break;
                         case 4:
                             printf("Listando todos os registros no heap:\n");
                             for (int i = 0; i < heap->tamanho; i++) {
                                 TRegistro reg;
                                 heap_ler_elemento(heap, i, &reg);
                                 if (strlen(reg.cpf) > 0) {
                                     printf("%s, %s, %d\n", reg.nome, reg.cpf, reg.nota);
                                 }
                             }
                             break;
                         case 5:
                             printf("Saindo do menu...\n");
                             heap_libera(heap);
                             break;
                     }
                 }
                 break;
        }
    }


    return 0;
}