#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"

// Função para criar um aluno
Taluno* criarAluno(long long cpf, const char* nome, int nota) {
    Taluno* aluno = malloc(sizeof(Taluno));
    if (!aluno) {
        fprintf(stderr, "Erro ao alocar memória para aluno.\n");
        exit(EXIT_FAILURE);
    }
    aluno->CPF = cpf;
    aluno->nome = strdup(nome); // Cópia da string
    aluno->nota = nota;
    return aluno;
}

// Função para liberar um aluno
void destruirAluno(Taluno* aluno) {
    if (aluno) {
        free(aluno->nome);
        free(aluno);
    }
}

// Função para ler e imprimir um aluno do arquivo hash
void imprimirAlunoArquivo(const Taluno* aluno, const hashSecundaria* hash) {
    FILE* arquivo = fopen(hash->nomeArquivo, "rb");
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir o arquivo %s para leitura.\n", hash->nomeArquivo);
        return;
    }
    Taluno alunoLido;
    int reg = (aluno->CPF) / 100;
    fseek(arquivo, hashar(reg - 1, hash->tamanho) * sizeof(Taluno), SEEK_SET);
    fread(&alunoLido, sizeof(Taluno), 1, arquivo);
    printf("Nome: %s\n", alunoLido.nome);
    printf("CPF: %lld\n", alunoLido.CPF);
    printf("Nota: %d\n", alunoLido.nota);
    fclose(arquivo);
}

int main() {
    long long cpf = 18476737712;
    int cpfTratado = cpf / 100;
    srand(cpfTratado);
    int numeroAleatorio = rand();

    hashSecundaria* hash = inicializa(100000);
    Taluno* aluno = criarAluno(cpf, "Joao", 10);

    inserir(hash, aluno);

    printf("9 primeiros digitos: %d\n", cpfTratado);
    printf("Numero aleatorio: %d\n", numeroAleatorio);
    printf("Arquivo da hash: %s\n", hash->nomeArquivo);

    imprimirAlunoArquivo(aluno, hash);

    destruirAluno(aluno);
    free(hash);
    return 0;
}