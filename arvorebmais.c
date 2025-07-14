#include "arvorebmais.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int cpf9(const char* cpf){
    char buf[10];
    strncpy(buf, cpf, 9);
    buf[9] = '\0';
    return atoi(buf);
}

static TNoArvBMais* criar_no_arvbmais(int folha){
    TNoArvBMais* n = (TNoArvBMais*)calloc(1, sizeof(TNoArvBMais));
    n->folha = folha;
    n->n_chaves = 0;
    n->prox = NULL;
    return n;
}

static void dividir_filho_arvbmais(TNoArvBMais* pai, int indice){
    TNoArvBMais* cheio = pai->filhos[indice];
    TNoArvBMais* novoNo = criar_no_arvbmais(cheio->folha);
    int t = ORDEM_BP;
    // move metade para novo nó
    novoNo->n_chaves = t;
    for(int i=0;i<t;i++){
        novoNo->chaves[i] = cheio->chaves[i+t];
        if(cheio->folha) novoNo->deslocs[i]=cheio->deslocs[i+t];
    }
    if(!cheio->folha){
        for(int i=0;i<t+1;i++){
            novoNo->filhos[i]=cheio->filhos[i+t];
        }
    }
    cheio->n_chaves = t;
    // abre espaço no pai
    for(int i=pai->n_chaves; i>indice; i--){
        pai->chaves[i] = pai->chaves[i-1];
        pai->filhos[i+1] = pai->filhos[i];
    }
    pai->chaves[indice] = novoNo->chaves[0];
    pai->filhos[indice+1]=novoNo;
    pai->n_chaves++;
    // liga folhas
    if(cheio->folha){
        novoNo->prox = cheio->prox;
        cheio->prox = novoNo;
    }
}

static void insere_nao_cheio_arvbmais(TNoArvBMais* no, int chave, long desloc){
    int i = no->n_chaves-1;
    if(no->folha){
        while(i>=0 && chave < no->chaves[i]){
            no->chaves[i+1]=no->chaves[i];
            no->deslocs[i+1]=no->deslocs[i];
            i--;
        }
        no->chaves[i+1]=chave;
        no->deslocs[i+1]=desloc;
        no->n_chaves++;
        return;
    }
    while(i>=0 && chave<no->chaves[i]) i--;
    i++;
    if(no->filhos[i]->n_chaves==2*ORDEM_BP){
        dividir_filho_arvbmais(no,i);
        if(chave>no->chaves[i]) i++;
    }
    insere_nao_cheio_arvbmais(no->filhos[i], chave, desloc);
}

// Função auxiliar para mesclar nós
static void mesclar_nos(TNoArvBMais* pai, int indice) {
    TNoArvBMais* esquerda = pai->filhos[indice];
    TNoArvBMais* direita = pai->filhos[indice + 1];
    int t = ORDEM_BP;
    
    // Move a chave do pai para a esquerda
    if (!esquerda->folha) {
        esquerda->chaves[t-1] = pai->chaves[indice];
    }
    
    // Move todas as chaves da direita para a esquerda
    for (int i = 0; i < direita->n_chaves; i++) {
        esquerda->chaves[t + i] = direita->chaves[i];
        if (esquerda->folha) {
            esquerda->deslocs[t + i] = direita->deslocs[i];
        }
    }
    
    // Move os filhos se não for folha
    if (!esquerda->folha) {
        for (int i = 0; i <= direita->n_chaves; i++) {
            esquerda->filhos[t + i] = direita->filhos[i];
        }
    }
    
    // Atualiza ponteiros de folhas
    if (esquerda->folha) {
        esquerda->prox = direita->prox;
    }
    
    // Remove a chave do pai
    for (int i = indice; i < pai->n_chaves - 1; i++) {
        pai->chaves[i] = pai->chaves[i + 1];
        pai->filhos[i + 1] = pai->filhos[i + 2];
    }
    
    esquerda->n_chaves += direita->n_chaves;
    if (!esquerda->folha) esquerda->n_chaves++;
    pai->n_chaves--;
    
    free(direita);
}

// Função para emprestar chave do irmão esquerdo
static void emprestar_da_esquerda(TNoArvBMais* pai, int indice) {
    TNoArvBMais* atual = pai->filhos[indice];
    TNoArvBMais* irmao = pai->filhos[indice - 1];
    
    // Move elementos para a direita
    for (int i = atual->n_chaves; i > 0; i--) {
        atual->chaves[i] = atual->chaves[i - 1];
        if (atual->folha) atual->deslocs[i] = atual->deslocs[i - 1];
    }
    
    if (!atual->folha) {
        for (int i = atual->n_chaves + 1; i > 0; i--) {
            atual->filhos[i] = atual->filhos[i - 1];
        }
        atual->filhos[0] = irmao->filhos[irmao->n_chaves];
        atual->chaves[0] = pai->chaves[indice - 1];
        pai->chaves[indice - 1] = irmao->chaves[irmao->n_chaves - 1];
    } else {
        atual->chaves[0] = irmao->chaves[irmao->n_chaves - 1];
        atual->deslocs[0] = irmao->deslocs[irmao->n_chaves - 1];
        pai->chaves[indice - 1] = atual->chaves[0];
    }
    
    atual->n_chaves++;
    irmao->n_chaves--;
}

// Função para emprestar chave do irmão direito
static void emprestar_da_direita(TNoArvBMais* pai, int indice) {
    TNoArvBMais* atual = pai->filhos[indice];
    TNoArvBMais* irmao = pai->filhos[indice + 1];
    
    if (!atual->folha) {
        atual->chaves[atual->n_chaves] = pai->chaves[indice];
        atual->filhos[atual->n_chaves + 1] = irmao->filhos[0];
        pai->chaves[indice] = irmao->chaves[0];
        
        // Move elementos do irmão para a esquerda
        for (int i = 0; i < irmao->n_chaves - 1; i++) {
            irmao->chaves[i] = irmao->chaves[i + 1];
        }
        for (int i = 0; i < irmao->n_chaves; i++) {
            irmao->filhos[i] = irmao->filhos[i + 1];
        }
    } else {
        atual->chaves[atual->n_chaves] = irmao->chaves[0];
        atual->deslocs[atual->n_chaves] = irmao->deslocs[0];
        pai->chaves[indice] = irmao->chaves[1];
        
        // Move elementos do irmão para a esquerda
        for (int i = 0; i < irmao->n_chaves - 1; i++) {
            irmao->chaves[i] = irmao->chaves[i + 1];
            irmao->deslocs[i] = irmao->deslocs[i + 1];
        }
    }
    
    atual->n_chaves++;
    irmao->n_chaves--;
}

// Função auxiliar para garantir que um nó tenha pelo menos t chaves
static void garantir_chaves_minimas(TNoArvBMais* pai, int indice) {
    if (indice != 0 && pai->filhos[indice - 1]->n_chaves >= ORDEM_BP) {
        emprestar_da_esquerda(pai, indice);
    } else if (indice != pai->n_chaves && pai->filhos[indice + 1]->n_chaves >= ORDEM_BP) {
        emprestar_da_direita(pai, indice);
    } else {
        if (indice != pai->n_chaves) {
            mesclar_nos(pai, indice);
        } else {
            mesclar_nos(pai, indice - 1);
        }
    }
}

// Função auxiliar para remover de um nó folha
static int remover_de_folha(TNoArvBMais* no, int chave) {
    int i = 0;
    while (i < no->n_chaves && no->chaves[i] != chave) i++;
    
    if (i == no->n_chaves) return 0; // Chave não encontrada
    
    // Move elementos para a esquerda
    for (int j = i; j < no->n_chaves - 1; j++) {
        no->chaves[j] = no->chaves[j + 1];
        no->deslocs[j] = no->deslocs[j + 1];
    }
    no->n_chaves--;
    return 1;
}

// Função auxiliar para remoção recursiva
static int remover_recursivo(TNoArvBMais* no, int chave) {
    if (no->folha) {
        return remover_de_folha(no, chave);
    }
    
    int i = 0;
    while (i < no->n_chaves && chave > no->chaves[i]) i++;
    
    if (no->filhos[i]->n_chaves == ORDEM_BP - 1) {
        garantir_chaves_minimas(no, i);
        // Reajustar i após possível mesclagem
        i = 0;
        while (i < no->n_chaves && chave > no->chaves[i]) i++;
    }
    
    return remover_recursivo(no->filhos[i], chave);
}

TARVBP* arvbp_inicializa(const char* nome_dados, const char* nome_indice){
    TARVBP* arvore = (TARVBP*)malloc(sizeof(TARVBP));
    arvore->raiz = criar_no_arvbmais(1);
    arvore->arquivoDados = fopen(nome_dados, "a+b");
    if(!arvore->arquivoDados){
        perror("Erro ao abrir dados_arvbmais.bin");
    }
    // Carregamento de índice não implementado (inicial vazio)
    (void)nome_indice;
    return arvore;
}

void arvbp_fecha(TARVBP* arvore, const char* nome_indice){
    // Persistência simples ainda não implementada: placeholder
    (void)nome_indice;
    if(arvore){
        fclose(arvore->arquivoDados);
        // TODO: liberar nós recursivamente
        free(arvore);
    }
}

void arvbp_insere(TARVBP* arvore, TRegistro* registro){
    // grava registro ao final do arquivo de dados
    fseek(arvore->arquivoDados, 0, SEEK_END);
    long desloc = ftell(arvore->arquivoDados);
    fwrite(registro, sizeof(TRegistro), 1, arvore->arquivoDados);
    fflush(arvore->arquivoDados);

    int chave = cpf9(registro->cpf);
    TNoArvBMais* r = arvore->raiz;
    if(r->n_chaves == 2*ORDEM_BP){
        TNoArvBMais* s = criar_no_arvbmais(0);
        arvore->raiz = s;
        s->filhos[0]=r;
        dividir_filho_arvbmais(s,0);
        int idx = (chave > s->chaves[0]) ? 1 : 0;
        insere_nao_cheio_arvbmais(s->filhos[idx], chave, desloc);
    } else {
        insere_nao_cheio_arvbmais(r, chave, desloc);
    }
}

int arvbp_busca(TARVBP* arvore, const char* cpf, TRegistro* out){
    int chave = cpf9(cpf);
    TNoArvBMais* no = arvore->raiz;
    while(no){
        int i=0;
        while(i<no->n_chaves && chave>no->chaves[i]) i++;
        if(no->folha){
            if(i<no->n_chaves && chave==no->chaves[i]){
                fseek(arvore->arquivoDados, no->deslocs[i], SEEK_SET);
                fread(out, sizeof(TRegistro),1, arvore->arquivoDados);
                // Verificar se o CPF completo confere
                if(strcmp(out->cpf, cpf) == 0) {
                    return 1;
                }
            }
            return 0;
        } else {
            no = no->filhos[i];
        }
    }
    return 0;
}

int arvbp_remove(TARVBP* arvore, const char* cpf){
    int chave = cpf9(cpf);
    
    // Verificar se a chave existe
    TRegistro temp;
    if (!arvbp_busca(arvore, cpf, &temp)) {
        return 0; // Chave não encontrada
    }
    
    int resultado = remover_recursivo(arvore->raiz, chave);
    
    // Se a raiz ficou vazia e não é folha, promover o único filho
    if (arvore->raiz->n_chaves == 0 && !arvore->raiz->folha) {
        TNoArvBMais* temp = arvore->raiz;
        arvore->raiz = arvore->raiz->filhos[0];
        free(temp);
    }
    
    return resultado;
} 