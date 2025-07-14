#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

int cpf9Primeiros(const char* cpf){
    char buffer[10];
    strncpy(buffer, cpf, 9);
    buffer[9] = '\0';
    return atoi(buffer);
}

int hashar(int valor, int tamanho){
    srand(valor);
    return (rand() % tamanho) * 2;
}

THashSecundaria* hash_carrega_existente(int tamanho){
    THashSecundaria* hash = (THashSecundaria*)malloc(sizeof(THashSecundaria));
    if (!hash) {
        printf("Erro ao alocar memória para hash\n");
        return NULL;
    }

    hash->arquivo = fopen("hash.bin", "r+b");
    if(!hash->arquivo) {
        printf("Erro ao abrir arquivo hash.bin\n");
        free(hash);
        return NULL;
    }
    
    hash->tamanho = tamanho;
    return hash; 
}

THashSecundaria* hash_inicializa(int tamanho){
    THashSecundaria* hash = (THashSecundaria*)malloc(sizeof(THashSecundaria));

    hash->arquivo = fopen("hash.bin", "r+b");

    if(hash->arquivo == NULL) {
        hash->arquivo = fopen("hash.bin", "w+b");
        if(hash->arquivo == NULL) {
            perror("Não foi possível criar o arquivo hash.bin");
            fclose(hash->arquivo);
            free(hash);
            exit(1);
        }
        TRegistro vazio = {0};
        for(int i = 0; i < tamanho; i++) {
            if (fwrite(&vazio, sizeof(TRegistro), 1, hash->arquivo) != 1) {
                printf("Falha ao inicializar o arquivo hash");
                fclose(hash->arquivo);
                free(hash);
                exit(1);
            }
        }
    }
    
    hash->tamanho = tamanho;
    return hash; 
}

void hash_libera(THashSecundaria* hash){
    if(hash) {
        if(hash->arquivo) {
            fclose(hash->arquivo);
        }
        free(hash);
    }
}

int contador_colisoes = 0;

void hash_resetar_colisoes() {
    contador_colisoes = 0;
}

int hash_obter_colisoes() {
    return contador_colisoes;
}

void hash_insere(THashSecundaria* hash, TRegistro* registro){
    if (!hash || !hash->arquivo) {
        printf("Erro: Hash ou arquivo não inicializado.\n");
        return;
    }

    int chave = cpf9Primeiros(registro->cpf);
    int posicaoOriginal = hashar(chave, hash->tamanho);
    int posicao = posicaoOriginal;
    
    TRegistro temp;
    
    fseek(hash->arquivo, posicao * sizeof(TRegistro), SEEK_SET);
    fread(&temp, sizeof(TRegistro), 1, hash->arquivo);

    if(strlen(temp.cpf) == 0 || strcmp(temp.cpf, registro->cpf) == 0) {
        fseek(hash->arquivo, posicao * sizeof(TRegistro), SEEK_SET);
        fwrite(registro, sizeof(TRegistro), 1, hash->arquivo);
        fflush(hash->arquivo);
        return;
    }

    contador_colisoes++;

    posicao = (posicao + 1) % hash->tamanho;

    while(posicao != posicaoOriginal) {
        fseek(hash->arquivo, posicao * sizeof(TRegistro), SEEK_SET);
        fread(&temp, sizeof(TRegistro), 1, hash->arquivo);

        if(strlen(temp.cpf) == 0 || strcmp(temp.cpf, registro->cpf) == 0) {
            fseek(hash->arquivo, posicao * sizeof(TRegistro), SEEK_SET);
            fwrite(registro, sizeof(TRegistro), 1, hash->arquivo);
            fflush(hash->arquivo);
            return;
        }
        
        posicao = (posicao + 1) % hash->tamanho;
    }
    
    printf("Hash cheia! Não foi possível inserir o registro com CPF: %s\n", registro->cpf);
}

int hash_busca(THashSecundaria* hash, const char* cpf, TRegistro* out){
    if (!hash || !hash->arquivo) return 0;

    int chave = cpf9Primeiros(cpf);
    int posicaoOriginal = hashar(chave, hash->tamanho);
    int posicao = posicaoOriginal;
    
    TRegistro temp;
    
    do {
        fseek(hash->arquivo, posicao * sizeof(TRegistro), SEEK_SET);
        if (fread(&temp, sizeof(TRegistro), 1, hash->arquivo) != 1) return 0;
        
        if(strcmp(temp.cpf, cpf) == 0) {
            *out = temp;
            return 1;
        }
        
        if(strlen(temp.cpf) == 0) {
            return 0;
        }
        
        posicao = (posicao + 1) % hash->tamanho;
        
    } while(posicao != posicaoOriginal);
    
    return 0;
}

int hash_remove(THashSecundaria* hash, const char* cpf){
    if (!hash || !hash->arquivo) return 0;

    int chave = cpf9Primeiros(cpf);
    int posicaoOriginal = hashar(chave, hash->tamanho);
    int posicao = posicaoOriginal;
    
    TRegistro temp;
    
    do {
        fseek(hash->arquivo, posicao * sizeof(TRegistro), SEEK_SET);
        if (fread(&temp, sizeof(TRegistro), 1, hash->arquivo) != 1) return 0;
        
        if(strcmp(temp.cpf, cpf) == 0) {
            TRegistro vazio = {0};
            fseek(hash->arquivo, posicao * sizeof(TRegistro), SEEK_SET);
            fwrite(&vazio, sizeof(TRegistro), 1, hash->arquivo);
            fflush(hash->arquivo);
            return 1;
        }
        
        if(strlen(temp.cpf) == 0) {
            return 0;
        }
        
        posicao = (posicao + 1) % hash->tamanho;
        
    } while(posicao != posicaoOriginal);
    
    return 0;
}

TRegistro* ler_registro_na_posicao(THashSecundaria *h, int pos) {
    if (!h || !h->arquivo) return NULL;
    
    TRegistro *registro = (TRegistro*)malloc(sizeof(TRegistro));
    if (!registro) return NULL;

    fseek(h->arquivo, pos * sizeof(TRegistro), SEEK_SET);
    if (fread(registro, sizeof(TRegistro), 1, h->arquivo) != 1) {
        free(registro);
        return NULL;
    }
    return registro;
}

void escrever_registro_na_posicao(THashSecundaria *h, int pos, TRegistro *r) {
    if (!h || !h->arquivo) return;

    fseek(h->arquivo, pos * sizeof(TRegistro), SEEK_SET);
    fwrite(r, sizeof(TRegistro), 1, h->arquivo);
    fflush(h->arquivo);
}
