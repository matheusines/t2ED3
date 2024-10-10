#ifndef CABECALHO_H
#define CABECALHO_H

#include <stdio.h>
#include <stdlib.h>

// Estrutura do cabeçalho para o arquivo binário
typedef struct {
    char status;            // Indica o status do arquivo: '0' se inconsistente e '1' se consistente
    int topo;               // RRN do primeiro registro removido logicamente ou -1 se não houver registros removidos
    int proxRRN;            // Próximo RRN disponível para inserção de registros
    int nroRegRem;          // Número de registros que foram logicamente removidos
    int nroPagDisco;        // Número de páginas de disco ocupadas pelo arquivo
    int qttCompacta;        // Contador de quantas vezes o arquivo foi compactado
} Cabecalho;


// Função que inicializa os campos do cabeçalho com valores padrões.
Cabecalho inicializarCabecalho();

// Função que escreve os campos do cabeçalho no início do arquivo binário.
// Essa função deve ser chamada sempre que o cabeçalho for atualizado, para refletir as mudanças no arquivo.
void escreverCabecalhoBin(FILE *fp, Cabecalho *header);

// Função que lê o cabeçalho do arquivo binário e preenche a estrutura Cabecalho com os valores lidos.
void lerCabecalhoBin(FILE *fp, Cabecalho *header);

#endif
