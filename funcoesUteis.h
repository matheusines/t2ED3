#ifndef FUNCOES_UTEIS_H
#define FUNCOES_UTEIS_H

#include <stdio.h>
#include <stdlib.h>

// Função auxiliar para ler uma string delimitada do arquivo binário
// A função lê caracteres até encontrar o delimitador ou o lixo, e armazena a string resultante em 'dest'.
// 'dest' deve ser um ponteiro previamente alocado com espaço suficiente para armazenar a string.
void lerString(FILE *arquivo, char *dest);

// Função para preencher o restante do espaço de um registro com caracteres de lixo ('$').
// Isso garante que o registro tenha exatamente 160 bytes, preenchendo o espaço não utilizado com '$'.
// 'tamanhoUsado' é o número de bytes já utilizados pelos dados do registro, e o restante será preenchido.
void preencherComLixo(FILE *arquivoBin, int tamanhoUsado);

#endif
