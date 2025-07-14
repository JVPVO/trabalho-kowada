#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

static int cpf9Primeiros(const char* cpf){
    char buffer[10];
    strncpy(buffer, cpf, 9);
    buffer[9] = '\0';
    return atoi(buffer);
}

THashSecundaria* hash_inicializa(int tamanho){
    // Cria o arquivo se não existir
    FILE* arquivo = fopen("hash.bin", "rb");
    if(!arquivo) {
        arquivo = fopen("hash.bin", "wb");
        if(arquivo) {
            // Inicializa arquivo com registros vazios
            TRegistro vazio = {0};
            for(int i = 0; i < tamanho; i++) {
                fwrite(&vazio, sizeof(TRegistro), 1, arquivo);
            }
            fclose(arquivo);
        }
    } else {
        fclose(arquivo);
    }
    
    THashSecundaria* hash = malloc(sizeof(THashSecundaria));
    hash->nomeArquivo = "hash.bin";
    hash->tamanho = tamanho;
    return hash; 
}

int hashar(int valor, int tamanho){
    return valor % tamanho;
}

void hash_insere(THashSecundaria* hash, TRegistro* registro){
    int chave = cpf9Primeiros(registro->cpf);
    int posicao = hashar(chave, hash->tamanho);
    
    FILE* arquivo = fopen(hash->nomeArquivo, "r+b");
    if(!arquivo){
        fprintf(stderr, "Erro ao abrir arquivo hash para inserção\n");
        return;
    }
    
    TRegistro temp;
    int posicaoOriginal = posicao;
    
    // Endereçamento aberto - busca posição livre ou com mesmo CPF
    do {
        fseek(arquivo, posicao * sizeof(TRegistro), SEEK_SET);
        fread(&temp, sizeof(TRegistro), 1, arquivo);
        
        // Se posição vazia ou mesmo CPF (sobrescrever)
        if(strlen(temp.cpf) == 0 || strcmp(temp.cpf, registro->cpf) == 0) {
            fseek(arquivo, posicao * sizeof(TRegistro), SEEK_SET);
            fwrite(registro, sizeof(TRegistro), 1, arquivo);
            fclose(arquivo);
            return;
        }
        
        // Próxima posição (circular)
        posicao = (posicao + 1) % hash->tamanho;
        
    } while(posicao != posicaoOriginal);
    
    fclose(arquivo);
    fprintf(stderr, "Hash cheia! Não foi possível inserir o registro.\n");
}

int hash_busca(THashSecundaria* hash, const char* cpf, TRegistro* out){
    int chave = cpf9Primeiros(cpf);
    int posicao = hashar(chave, hash->tamanho);
    
    FILE* arquivo = fopen(hash->nomeArquivo, "rb");
    if(!arquivo){
        fprintf(stderr, "Erro ao abrir arquivo hash para busca\n");
        return 0;
    }
    
    TRegistro temp;
    int posicaoOriginal = posicao;
    
    do {
        fseek(arquivo, posicao * sizeof(TRegistro), SEEK_SET);
        fread(&temp, sizeof(TRegistro), 1, arquivo);
        
        // Se encontrou o CPF
        if(strcmp(temp.cpf, cpf) == 0) {
            *out = temp;
            fclose(arquivo);
            return 1;
        }
        
        // Se posição vazia, não existe
        if(strlen(temp.cpf) == 0) {
            fclose(arquivo);
            return 0;
        }
        
        // Próxima posição (circular)
        posicao = (posicao + 1) % hash->tamanho;
        
    } while(posicao != posicaoOriginal);
    
    fclose(arquivo);
    return 0; // Não encontrado
}

int hash_remove(THashSecundaria* hash, const char* cpf){
    int chave = cpf9Primeiros(cpf);
    int posicao = hashar(chave, hash->tamanho);
    
    FILE* arquivo = fopen(hash->nomeArquivo, "r+b");
    if(!arquivo){
        fprintf(stderr, "Erro ao abrir arquivo hash para remoção\n");
        return 0;
    }
    
    TRegistro temp;
    int posicaoOriginal = posicao;
    
    do {
        fseek(arquivo, posicao * sizeof(TRegistro), SEEK_SET);
        fread(&temp, sizeof(TRegistro), 1, arquivo);
        
        // Se encontrou o CPF
        if(strcmp(temp.cpf, cpf) == 0) {
            // Marca como removido (zera o registro)
            TRegistro vazio = {0};
            fseek(arquivo, posicao * sizeof(TRegistro), SEEK_SET);
            fwrite(&vazio, sizeof(TRegistro), 1, arquivo);
            fclose(arquivo);
            return 1;
        }
        
        // Se posição vazia, não existe
        if(strlen(temp.cpf) == 0) {
            fclose(arquivo);
            return 0;
        }
        
        // Próxima posição (circular)
        posicao = (posicao + 1) % hash->tamanho;
        
    } while(posicao != posicaoOriginal);
    
    fclose(arquivo);
    return 0; // Não encontrado
}

void hash_libera(THashSecundaria* hash){
    if(hash) {
        free(hash);
    }
}
