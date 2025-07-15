
Trabalho feito por: João Vitor Pires Vilela de Oliveira

## Como compilar e rodar o código:

No diretório raiz temos dois arquivos .ps1 que rodam no powershell, que compilam e rodam os programas, um para o script de geração de registros e outro para o main, onde temos as estruturas de dados implementadas na memória secundária.

Os scripts são:
Geração de Registros:
```bash 
gcc -o gerarRegistros gerarRegistros.c
./gerarRegistros
```
Rodar a main, o chcp 65001 foi a solução para garantir que os acentos e pontuações sejam corretamente exibidos no terminal:
```bash 
chcp 65001
gcc -o main main.c hash.c arvorebmais.c heap.c
./main
```

## Geração de Registros

```bash 
gcc -o gerarRegistros gerarRegistros.c && ./gerarRegistros
```

A maior dificuldade mesmo, estabelecido que os CPFs não precisam ser válidos é a geração de nomes; para isso, peguei 20 nomes e 20 sobrenomes aleatórios em um site e da minha cabeça. Com isso a gente faz os registros com uma função rand() simples

```c 
int idxNome = rand() % 20;
int idxSobrenome = rand() % 20;
sprintf(nome, "%s %s", nomes[idxNome], sobrenomes[idxSobrenome]);
```

Com isso perguntamos ao usuário quantos registros ele quer gerar, e geramos nota e CPFs com funções simples de gerações de números.

## Hash

### Implementação

A implementação de Hash na memória secundária segue a seguinte lógica:
Geramos a chave como foi especificado em sala de aula através da função "hashar"
```c 
int hashar(int valor, int tamanho){
    srand(valor);
    return (rand() % tamanho) * 2;
}
```
Após isso alocamos o tipo de Hash Secundária, um tipo criado no hash.h que tem como parâmetros um ponteiro tipo FILE para o arquivo da hash secundária, além de um inteiro que representa o tamanho da hash. Após isso inicializamos o arquivo com registros vazios e retornamos nosso tipo Hash Secundária

Como a função rand() tem um limite de 32.768 números possíveis a serem gerados e como especificado na descrição do trabalho, nossa tabela hash possui 100.000 posições eu multiplico o valor obtido por 2 para "espalhar" os registros. 
### O tratamento de colisões

Como mencionado anteriormente, espalhamos os registros, dessa forma, possuímos um espaço entre o registro n e n+1 ao qual:
$$\begin{bmatrix} n & 0 & 0 & n+1 \end{bmatrix}$$
Com isso, temos duas "casas" nulas, dessa forma o tratamento de colisões implementado é um endereçamento aberto com tentativa linear, onde após uma colisão, vamos ao próximo espaço, somando + 1 na posição.

### Operações básicas

Mais detalhes das operações básicas são encontradas no código, mas utilizamos total proveito da função
```c 
fseek(h->arquivo, pos * sizeof(TRegistro), SEEK_SET);
```
Onde multiplicamos a posição do registro pelo sizeof(TRegistro), para um acesso rápido à posição desejada, sem ter que iterar pelo arquivo todo.

Compreendendo essa lógica, é simples recriar as operações de hash nessa implementação de memória secundária

## Heap

A estrutura da heap foi implementada como um heap máximo, onde a prioridade é a nota como especificado na descrição do trabalho.

```c 
void heap_trocar_arquivo(THeap* heap, int i, int j) {
    if (!heap || i == j || i < 0 || j < 0 || i >= heap->tamanho || j >= heap->tamanho) {
        return;
    }
    TRegistro temp1, temp2;
    heap_ler_elemento(heap, i, &temp1);
    heap_ler_elemento(heap, j, &temp2);
    if (strlen(temp1.cpf) == 0 || strlen(temp2.cpf) == 0) {
        return;
    }
    heap_escrever_elemento(heap, i, &temp2);
    heap_escrever_elemento(heap, j, &temp1);
}
```

Foram implementadas as funções base de uma heap, inserção, remoção, leitura, heapify, feitas em memória secundária, como o exemplo acima. Mais detalhes sobre a implementação estão no código.


## Árvore B+

Essa é a estrutura do TARVBP, tipo árvore b+:
```c 
typedef struct arvbmais {
    TNoArvBMais* raiz;
    FILE* arquivoDados;
} TARVBP;
```
E o tipo nó:
```c 
typedef struct no_arvbmais {
    int folha;                              // Indica se é folha (1) ou não (0)
    int n_chaves;                           // Número de chaves no nó
    int chaves[2*ORDEM_BP];                 // Array de chaves (primeiros 9 dígitos do CPF)
    long deslocs[2*ORDEM_BP];               // Deslocamentos no arquivo de dados
    struct no_arvbmais* filhos[2*ORDEM_BP+1]; // Ponteiros para filhos
    struct no_arvbmais* prox;               // Ponteiro para próximo nó folha
} TNoArvBMais;
```

Para as operações da árvore b+ que é sensível a posicionamento e tem uma estrutura mais complexa, é necessário dar proveito das funções de posicionamento de arquivo, esse é um exemplo com a função de inserção principal da implementação 
```c 
void arvbp_insere(TARVBP* arvore, TRegistro* registro){
    // Primeiro, salva o registro no arquivo de dados
    fseek(arvore->arquivoDados, 0, SEEK_END);
    long desloc = ftell(arvore->arquivoDados);
    fwrite(registro, sizeof(TRegistro), 1, arvore->arquivoDados);
    fflush(arvore->arquivoDados);
    //  Extrai a chave (9 primeiros dígitos do CPF)
    int chave = cpf9(registro->cpf);
    //  Insere na árvore B+
    TNoArvBMais* r = arvore->raiz;
    if(r->n_chaves == 2*ORDEM_BP){
        // Raiz está cheia, precisa dividir
        TNoArvBMais* s = criar_no_arvbmais(0);
        arvore->raiz = s;
        s->filhos[0] = r;
        dividir_filho_arvbmais(s, 0);
        int idx = (chave > s->chaves[0]) ? 1 : 0;
        insere_nao_cheio_arvbmais(s->filhos[idx], chave, desloc);
    } else {
        insere_nao_cheio_arvbmais(r, chave, desloc);
    }
}
```

De resto estão implementadas as outras funções da árvore b+ também, para evitar poluir a documentação, tentei reduzir a quantidade de linhas de código na documentação.