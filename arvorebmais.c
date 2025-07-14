#include "arvorebmais.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int cpf9(const char* cpf){
    char buf[10];
    strncpy(buf, cpf, 9);
    buf[9] = '\0';
    return atoi(buf);
}

TNoArvBMais* criar_no_arvbmais(int folha){
    TNoArvBMais* n = (TNoArvBMais*)calloc(1, sizeof(TNoArvBMais));
    n->folha = folha;
    n->n_chaves = 0;
    n->prox = NULL;
    return n;
}

void dividir_filho_arvbmais(TNoArvBMais* pai, int indice){
    TNoArvBMais* cheio = pai->filhos[indice];
    TNoArvBMais* novoNo = criar_no_arvbmais(cheio->folha);
    int t = ORDEM_BP;
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
    for(int i=pai->n_chaves; i>indice; i--){
        pai->chaves[i] = pai->chaves[i-1];
        pai->filhos[i+1] = pai->filhos[i];
    }
    pai->chaves[indice] = novoNo->chaves[0];
    pai->filhos[indice+1]=novoNo;
    pai->n_chaves++;
    if(cheio->folha){
        novoNo->prox = cheio->prox;
        cheio->prox = novoNo;
    }
}

void insere_nao_cheio_arvbmais(TNoArvBMais* no, int chave, long desloc){
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

TARVBP* arvbp_carrega_existente(const char* nome_dados, const char* nome_indice){
    TARVBP* arvore = (TARVBP*)malloc(sizeof(TARVBP));
    arvore->raiz = criar_no_arvbmais(1);
    arvore->arquivoDados = fopen(nome_dados, "r+b");
    if(!arvore->arquivoDados){
        printf("Erro ao abrir dados_arvbmais.bin");
        free(arvore);
        return NULL;
    }
    
    // Reconstrói os índices a partir dos dados existentes
    fseek(arvore->arquivoDados, 0, SEEK_SET);
    TRegistro registro;
    long posicao = 0;
    
    while(fread(&registro, sizeof(TRegistro), 1, arvore->arquivoDados) == 1) {
        if(strlen(registro.cpf) > 0) {
            int chave = cpf9(registro.cpf);
            TNoArvBMais* r = arvore->raiz;
            if(r->n_chaves == 2*ORDEM_BP){
                TNoArvBMais* s = criar_no_arvbmais(0);
                arvore->raiz = s;
                s->filhos[0]=r;
                dividir_filho_arvbmais(s,0);
                int idx = (chave > s->chaves[0]) ? 1 : 0;
                insere_nao_cheio_arvbmais(s->filhos[idx], chave, posicao);
            } else {
                insere_nao_cheio_arvbmais(r, chave, posicao);
            }
        }
        posicao = ftell(arvore->arquivoDados);
    }
    
    (void)nome_indice;
    return arvore;
}

TARVBP* arvbp_inicializa(const char* nome_dados, const char* nome_indice){
    TARVBP* arvore = (TARVBP*)malloc(sizeof(TARVBP));
    arvore->raiz = criar_no_arvbmais(1);
    arvore->arquivoDados = fopen(nome_dados, "a+b");
    if(!arvore->arquivoDados){
        printf("Erro ao abrir dados_arvbmais.bin");
    }
    (void)nome_indice;
    return arvore;
}

void arvbp_fecha(TARVBP* arvore, const char* nome_indice){
    (void)nome_indice;
    if(arvore){
        fclose(arvore->arquivoDados);
        free(arvore);
    }
}

void arvbp_insere(TARVBP* arvore, TRegistro* registro){
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
    
    TNoArvBMais* no = arvore->raiz;
    while(no){
        int i=0;
        while(i<no->n_chaves && chave>no->chaves[i]) i++;
        if(no->folha){
            if(i<no->n_chaves && chave==no->chaves[i]){
                TRegistro temp;
                fseek(arvore->arquivoDados, no->deslocs[i], SEEK_SET);
                fread(&temp, sizeof(TRegistro), 1, arvore->arquivoDados);
                if(strcmp(temp.cpf, cpf) == 0) {
                    for(int j = i; j < no->n_chaves - 1; j++){
                        no->chaves[j] = no->chaves[j+1];
                        no->deslocs[j] = no->deslocs[j+1];
                    }
                    no->n_chaves--;
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