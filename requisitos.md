Aberto: terça-feira, 1 jul. 2025, 00:00
Vencimento: segunda-feira, 14 jul. 2025, 23:59

- Gerar uma lista de 10.000 registros aleatórios onde cada registro possui nome (50 caracteres), CPF (11 dígitos consecutivos) e  nota (de 0 a 100). Estes registros podem ficar armazenados num arquivo inicial na ordem quem são gerados.

1) Armazene os registros num arquivo com estrutura B+ (com os índices em outro arquivo).

2) Armazene os registros num arquivo numa estrutura Hash com endereçamento aberto. A função hash deve ser calculada a partir da função rand(  ). A chave é passada para a função srand(  ). O tamanho do resultado da hash pode ser 100.000.

3) Armazene os registros num arquivo com estrutura heap. A prioridade é a nota.

- Implemente as operações de inserção, consulta e remoção para cada estrutura.

obs. O CPF não precisa ser válido. A chave é formada pelos 9 primeiros dígitos do CPF.
Os programas devem ser implementados em C. Junto com os programas, deve ser entregue uma documentação e arquivos de exemplo.