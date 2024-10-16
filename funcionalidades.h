#ifndef FUNCIONALIDADES_H
#define FUNCIONALIDADES_H

// Declaração das funções principais que implementam as funcionalidades do Trabalho 1.

// Função responsável pela leitura de um arquivo CSV e armazenamento em um arquivo binário.
void funcionalidade1();

// Função que realiza a leitura do arquivo binário e exibe os registros que não foram removidos.
void funcionalidade2();

// Função que realiza a busca de registros no arquivo binário com base em um campo e valor fornecidos pelo usuário.
void funcionalidade3();

// Função que realiza a remoção lógica de registros no arquivo binário.
void funcionalidade4();

// Função que permite a inserção de novos registros no arquivo binário, reutilizando o espaço de registros removidos, se disponível.
void funcionalidade5();

// Função que realiza a compactação do arquivo binário, removendo fisicamente os registros logicamente removidos.
void funcionalidade6();

// Função que cria um índice árvore-B baseado no campo 'nome' de um arquivo de dados existente.
void funcionalidade7();

// Função que realiza a busca de registros no arquivo de dados utilizando o índice árvore-B no campo 'nome'.
void funcionalidade8();

// Função que insere novos registros no arquivo de dados e atualiza o índice árvore-B correspondente com a chave do registro inserido.
void funcionalidade9();

#endif // FUNCIONALIDADES_H
