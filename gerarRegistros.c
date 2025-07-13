#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/*
Script usado para gerar os 10.000 registros requisitados no trabalho, não sei se precisava ser em C, ou se podíamos
gerar os registros em outras linguagens, com bibliotecas de geração de dados aleatórios válidos, o que deixaria mais bonito.
*/ 

typedef struct {
    char nome[51];
    char cpf[12];
    int nota;
} Registro;

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

int main() {
    srand(time(NULL));

    const char *nomeArquivo = "registros.txt";
    FILE *arquivo;

    arquivo = fopen(nomeArquivo, "w");

    if (arquivo == NULL) {
        perror("Erro");
        return 1;
    }

    int NUM_REGISTROS;
    printf("Digite o número de registros a serem gerados: ");
    scanf("%d", &NUM_REGISTROS);

    for (int i = 0; i < NUM_REGISTROS; i++) {
        Registro novoRegistro;

        gerarNomeAleatorio(novoRegistro.nome);
        gerarCpfAleatorio(novoRegistro.cpf);
        novoRegistro.nota = rand() % 101;

        fprintf(arquivo, "%s,%s,%d\n", novoRegistro.nome, novoRegistro.cpf, novoRegistro.nota);

        if ((i + 1) % 1000 == 0) {
            printf("%d...\n", i + 1);
        }
    }

    fclose(arquivo);

    printf("Concluído.\n");

    return 0;
}