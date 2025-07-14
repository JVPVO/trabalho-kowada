#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

// Extrai os 9 primeiros dígitos do CPF e converte para um inteiro.
int cpf9Primeiros(const char* cpf){
    char buffer[10];
    strncpy(buffer, cpf, 9);
    buffer[9] = '\0';
    return atoi(buffer);
}

// Função de hash simplificada e mais rápida.
int hashar(int valor, int tamanho){
    srand(valor);
    return (rand() % tamanho) * 2;
}

// Inicializa a tabela hash. Abre o arquivo binário e o mantém aberto.
THashSecundaria* hash_inicializa(int tamanho){
    THashSecundaria* hash = (THashSecundaria*)malloc(sizeof(THashSecundaria));

    // Tenta abrir o arquivo para leitura e escrita binária.
    hash->arquivo = fopen("hash.bin", "r+b");

    // Se o arquivo não existe, cria e o inicializa.
    if(hash->arquivo == NULL) {
        hash->arquivo = fopen("hash.bin", "w+b");
        if(hash->arquivo == NULL) {
            perror("Não foi possível criar o arquivo hash.bin");
            fclose(hash->arquivo);
            free(hash);
            exit(1);
        }
        // Inicializa o arquivo com registros vazios.
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

// Libera a memória da hash e fecha o arquivo.
void hash_libera(THashSecundaria* hash){
    if(hash) {
        if(hash->arquivo) {
            fclose(hash->arquivo);
        }
        free(hash);
    }
}

// Contador de colisões
int contador_colisoes = 0;

void hash_resetar_colisoes() {
    contador_colisoes = 0;
}

int hash_obter_colisoes() {
    return contador_colisoes;
}

// Insere um registro na tabela hash usando sondagem linear.
void hash_insere(THashSecundaria* hash, TRegistro* registro){
    if (!hash || !hash->arquivo) {
        fprintf(stderr, "Erro: Hash ou arquivo não inicializado.\n");
        return;
    }

    int chave = cpf9Primeiros(registro->cpf);
    int posicaoOriginal = hashar(chave, hash->tamanho);
    int posicao = posicaoOriginal;
    
    TRegistro temp;
    
    // Checa a primeira posição
    fseek(hash->arquivo, posicao * sizeof(TRegistro), SEEK_SET);
    fread(&temp, sizeof(TRegistro), 1, hash->arquivo);

    // Se a posição está livre ou é o mesmo registro, insere direto.
    if(strlen(temp.cpf) == 0 || strcmp(temp.cpf, registro->cpf) == 0) {
        fseek(hash->arquivo, posicao * sizeof(TRegistro), SEEK_SET);
        fwrite(registro, sizeof(TRegistro), 1, hash->arquivo);
        fflush(hash->arquivo);
        return;
    }

    // Se chegou aqui, a primeira posição estava ocupada por outra pessoa. É uma colisão.
    contador_colisoes++;

    // Inicia a sondagem a partir da próxima posição.
    posicao = (posicao + 1) % hash->tamanho;

    while(posicao != posicaoOriginal) {
        fseek(hash->arquivo, posicao * sizeof(TRegistro), SEEK_SET);
        fread(&temp, sizeof(TRegistro), 1, hash->arquivo);

        // Se encontrou um lugar vazio ou o mesmo registro para sobrescrever
        if(strlen(temp.cpf) == 0 || strcmp(temp.cpf, registro->cpf) == 0) {
            fseek(hash->arquivo, posicao * sizeof(TRegistro), SEEK_SET);
            fwrite(registro, sizeof(TRegistro), 1, hash->arquivo);
            fflush(hash->arquivo);
            return; // Inserção concluída.
        }
        
        // Continua a sondagem
        posicao = (posicao + 1) % hash->tamanho;
    }
    
    // Se o loop terminar, a hash está cheia.
    fprintf(stderr, "Hash cheia! Não foi possível inserir o registro com CPF: %s\n", registro->cpf);
}


// Busca um registro na tabela hash pelo CPF.
int hash_busca(THashSecundaria* hash, const char* cpf, TRegistro* out){
    if (!hash || !hash->arquivo) return 0;

    int chave = cpf9Primeiros(cpf);
    int posicaoOriginal = hashar(chave, hash->tamanho);
    int posicao = posicaoOriginal;
    
    TRegistro temp;
    
    do {
        fseek(hash->arquivo, posicao * sizeof(TRegistro), SEEK_SET);
        if (fread(&temp, sizeof(TRegistro), 1, hash->arquivo) != 1) return 0; // Falha na leitura
        
        // Se encontrou o CPF.
        if(strcmp(temp.cpf, cpf) == 0) {
            *out = temp;
            return 1; // Encontrado.
        }
        
        // Se encontrou uma posição vazia, o registro não existe.
        if(strlen(temp.cpf) == 0) {
            return 0; // Não encontrado.
        }
        
        // Próxima posição.
        posicao = (posicao + 1) % hash->tamanho;
        
    } while(posicao != posicaoOriginal);
    
    return 0; // Não encontrado após varrer a tabela.
}

// Remove um registro da tabela hash.
int hash_remove(THashSecundaria* hash, const char* cpf){
    if (!hash || !hash->arquivo) return 0;

    int chave = cpf9Primeiros(cpf);
    int posicaoOriginal = hashar(chave, hash->tamanho);
    int posicao = posicaoOriginal;
    
    TRegistro temp;
    
    do {
        fseek(hash->arquivo, posicao * sizeof(TRegistro), SEEK_SET);
        if (fread(&temp, sizeof(TRegistro), 1, hash->arquivo) != 1) return 0;
        
        // Se encontrou o CPF.
        if(strcmp(temp.cpf, cpf) == 0) {
            // Marca como removido (escreve um registro vazio).
            TRegistro vazio = {0};
            fseek(hash->arquivo, posicao * sizeof(TRegistro), SEEK_SET);
            fwrite(&vazio, sizeof(TRegistro), 1, hash->arquivo);
            fflush(hash->arquivo);
            return 1; // Removido com sucesso.
        }
        
        // Se encontrou uma posição vazia, o registro não existe.
        if(strlen(temp.cpf) == 0) {
            return 0; // Não encontrado.
        }
        
        // Próxima posição.
        posicao = (posicao + 1) % hash->tamanho;
        
    } while(posicao != posicaoOriginal);
    
    return 0; // Não encontrado.
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
