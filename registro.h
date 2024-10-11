#ifndef REGISTRO_H
#define REGISTRO_H
#include <stdio.h>
#include <stdlib.h>
#include "cabecalho.h"

#define ORDEM 5

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

typedef struct  {
    
    char folha;
    int nroChavesIndexadas;
    int RRNdoNo;
    int ponteiros [ORDEM];
    long chaves [ORDEM-1];
    long referencias [ORDEM-1];

} RegistroArvoreB;


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
 * @param df Ponteiro para o arquivo binário onde o registro será inserido.
 * @param r Ponteiro para o registro a ser inserido.
 * @param header Cabeçalho do arquivo binário que será atualizado.
 */
void inserirRegistro(FILE *arquivo, Registro *r, Cabecalho *header);

/**
 * Função para compactar um arquivo binário, removendo registros logicamente removidos e gerando um novo arquivo compactado.
 * @param arquivoOriginal Ponteiro para o arquivo original a ser compactado.
 * @param arquivoCompactado Ponteiro para o arquivo compactado que será gerado.
 */
void compactarArquivoBinario(FILE *arquivoOriginal, FILE *arquivoCompactado);



#endif