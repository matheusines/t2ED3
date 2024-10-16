#ifndef REGISTROS_H
#define REGISTROS_H
#include <stdio.h>
#include <stdlib.h>
#include "cabecalhos.h"

#define TAM_PAGINA 93

typedef struct {
    // Campos de tamanho fixo
    char removido;          // '0' não removido, '1' removido logicamente (Campo de controle)
    int encadeamento;       // Próximo na lista de registros removidos (Campo de controle)
    int populacao;          // População da espécie
    float tamanho;          // Tamanho do indivíduo
    char unidadeMedida;     // Unidade de medida (char único)
    int velocidade;         // Velocidade do indivíduo
    // Campos de tamanho variável 
    char *nome;             // Nome da espécie
    char *especie;          // Nome científico da espécie
    char *habitat;          // Habitat da espécie
    char *tipo;             // Tipo da espécie
    char *dieta;            // Dieta da espécie
    char *alimento;         // Alimento da espécie
} Registro;

/**
 * Função para liberar a memória alocada para as strings do registro e o próprio registro.
 * @param reg Ponteiro para o registro que deve ser liberado.
 */
void liberarRegistro(Registro *reg);

/**
 * Função para criar um novo registro com valores padrão, inicializando os campos fixos e alocando memória para as strings.
 * @return Ponteiro para o novo registro ou NULL em caso de erro.
 */
Registro *criarRegistro();

/**
 * Função para escrever um registro em um arquivo binário.
 * Escreve todos os campos do registro, e preenche o espaço não utilizado com caracteres de lixo.
 * @param fp Ponteiro para o arquivo binário.
 * @param reg Ponteiro para o registro que será escrito.
 */
void escreverRegistro(FILE *arquivo, Registro *reg);

/**
 * Função para ler um registro de um arquivo binário.
 * Ignora registros logicamente removidos e pula o espaço de lixo.
 * @param fp Ponteiro para o arquivo binário.
 * @return Ponteiro para o registro lido, ou NULL em caso de erro ou fim de arquivo.
 */
Registro *lerRegistroBin(FILE *arquivo);

/**
 * Função para ler um registro a partir de um arquivo CSV (texto).
 * Interpreta os delimitadores e converte os campos nulos em valores padrões.
 * @param fp Ponteiro para o arquivo CSV.
 * @return Ponteiro para o registro lido, ou NULL em caso de erro ou fim de arquivo.
 */
Registro *lerRegistroCSV(FILE *arquivo);

/**
 * Função para buscar um registro com base em um campo específico e valor.
 * Compara o campo do registro com o valor fornecido, retornando 1 se houver uma correspondência.
 * @param r Ponteiro para o registro que está sendo comparado.
 * @param campo Nome do campo que será buscado.
 * @param valor Valor a ser comparado com o campo do registro.
 * @return 1 se o registro corresponder, 0 caso contrário.
 */
int buscarRegistro(Registro *r, const char *campo, const char *valor);

/**
 * Função para remover logicamente um registro, marcando-o como removido e encadeando-o na lista de registros removidos.
 * @param r Ponteiro para o registro a ser removido.
 * @param arquivoBin Ponteiro para o arquivo binário onde o registro será atualizado.
 * @param topo RRN do topo da lista de registros removidos.
 * @param byteOffset Offset do registro dentro do arquivo.
 * @param nroRegRem Ponteiro para o número de registros removidos (será incrementado).
 */
void removeRegistro(Registro *r, FILE *arquivoBin, int topo, long int byteOffset, int *nroRegRem);

/**
 * Função para inserir um registro no arquivo binário.
 * Reutiliza o espaço de registros removidos, se disponível, ou adiciona no final.
 * @param arquivo Ponteiro para o arquivo binário onde o registro será inserido.
 * @param r Ponteiro para o registro a ser inserido.
 * @param header Cabeçalho do arquivo binário que será atualizado.
 */
long inserirRegistro(FILE *arquivo, Registro *r, Cabecalho *header);

/**
 * Função para compactar um arquivo binário, removendo registros logicamente removidos e gerando um novo arquivo compactado.
 * @param arquivoOriginal Ponteiro para o arquivo original a ser compactado.
 * @param arquivoCompactado Ponteiro para o arquivo compactado que será gerado.
 */
void compactarArquivoBinario(FILE *arquivoOriginal, FILE *arquivoCompactado);



/*

**************************************************************************************************************************************
*                                                                                                                                    *
*                                                             TRABALHO 2                                                             *
*                                                                                                                                    *
**************************************************************************************************************************************

*/

#define ORDEM 5

typedef struct {

    char folha;                // Indica se o nó é folha ou não. Valores possíveis: 0 (não é folha) e 1 (é folha).
    int nroChavesIndexadas;    // Armazena o número de chaves de busca atualmente indexadas no nó.
    int RRNdoNo;               // Número do Registro Relativo (RRN) do nó, que é o identificador do nó no arquivo.
    int ponteiros[ORDEM+1];    // Array de ponteiros para subárvores, cada um representado por um valor de RRN. 
                               // Ponteiros vazios são representados por -1.
    long chaves[ORDEM];        // Array de chaves de busca armazenadas no nó, ordenadas. Chaves vazias são representadas por -1.
    long referencias[ORDEM];   // Array de referências para registros de dados no arquivo principal, correspondendo às chaves.

} Pagina;

/**
 * Função para criar uma nova página, inicializando os campos com valores padrão
 * @return Ponteiro para a nova página ou NULL em caso de erro.
 */
Pagina *criaPagina();

/**
 * Função para escrever uma pagina no arquivo indice.
 * Escreve todos os campos da pagina
 * @param fp Ponteiro para o arquivo indice.
 * @param pag Ponteiro para a página que será escrita.
 */
void escrevePaginaBin(FILE *fp, Pagina *pag);


/**
 * Função para ler uma pagina de um arquivo indice.
 * @param fp Ponteiro para o arquivo indice.
 * @return Ponteiro para a pagina lida, ou NULL em caso de erro ou fim de arquivo.
 */
Pagina *lePaginaBin(FILE *fp);

#endif