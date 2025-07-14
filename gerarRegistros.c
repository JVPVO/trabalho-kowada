#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "registro.h"

void gerarNomeAleatorio(char *nome) {
    const char *nomes[] = {
        "Ana", "Bruno", "Carlos", "Daniela", "Eduardo", "Fernando", "Gabriel", "Helena", "Igor", "Juliana",
        "João Vitor", "Larissa", "Marcos", "Natália", "Otávio", "Patrícia", "Teresa", "Rafael", "Sabrina", "Thiago"
    };
    const char *sobrenomes[] = {
        "Almeida", "Barbosa", "Cardoso", "Dias", "Esteves", "Ferreira", "Gomes", "Henrique", "Silveira", "Jardim",
        "Ramos", "Lima", "Mendes", "Nascimento", "Oliveira", "Pereira", "Queiroz", "Ramos", "Silva", "Teixeira"
    };

    int idxNome = rand() % 20;
    int idxSobrenome = rand() % 20;

    sprintf(nome, "%s %s", nomes[idxNome], sobrenomes[idxSobrenome]);
}

void gerarCpfAleatorio(char *cpf) {
    for (int i = 0; i < 11; i++) {
        cpf[i] = (rand() % 10) + '0';
    }
    cpf[11] = '\0';
}

#define NUM_REGISTROS 10000

int main() {
    srand((unsigned)time(NULL));

    const char *nomeArquivo = "registros.txt";
    FILE *arquivo = fopen(nomeArquivo, "w");
    if (!arquivo) {
        perror("Erro ao criar registros.txt");
        return 1;
    }

    for (int i = 0; i < NUM_REGISTROS; i++) {
        TRegistro novoRegistro;

        gerarNomeAleatorio(novoRegistro.nome);
        gerarCpfAleatorio(novoRegistro.cpf);
        novoRegistro.nota = rand() % 101;

        fprintf(arquivo, "%s,%s,%d\n", novoRegistro.nome, novoRegistro.cpf, novoRegistro.nota);

        if ((i + 1) % 1000 == 0) {
            printf("%d registros gerados...\n", i + 1);
        }
    }

    fclose(arquivo);
    printf("Arquivo registros.txt gerado com %d registros.\n", NUM_REGISTROS);
    return 0;
}