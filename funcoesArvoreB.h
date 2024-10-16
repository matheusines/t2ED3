#ifndef FUNCOES_ARVORE_B_H
#define FUNCOES_ARVORE_B_H

#include <stdio.h>
#include <stdlib.h>
#include "registros.h"
#include "cabecalhos.h"

#define NAO_ENCONTRADO 0
#define ENCONTRADO 1
#define ERRO -1
#define SEM_PROMOCAO 0
#define PROMOCAO 1
#define ORDEM 5

/**
 * Função para buscar uma chave na árvore-B.
 * Realiza uma busca recursiva para encontrar a chave em um nó específico da árvore.
 * @param arqIndice Ponteiro para o arquivo de índice onde a árvore-B é armazenada.
 * @param RRNAtual Número do registro relativo do nó atual.
 * @param chave Chave de busca desejada.
 * @return Retorna a referência associada à chave encontrada ou um valor indicando que a chave não foi encontrada.
 */
long busca(FILE *arqIndice, int RRNAtual, long chave);

/**
 * Função para inserir uma chave e referência na árvore-B.
 * Insere a chave no nó apropriado e, se necessário, realiza divisões de nós (split) promovendo chaves.
 * @param arqIndice Ponteiro para o arquivo de índice onde a árvore-B é armazenada.
 * @param RRNAtual Número do registro relativo do nó atual.
 * @param chave Chave a ser inserida.
 * @param referencia Referência associada à chave.
 * @param chavePromovida Ponteiro para armazenar a chave promovida em caso de divisão de nós.
 * @param referenciaPromovida Ponteiro para armazenar a referência promovida em caso de divisão de nós.
 * @param filhoDireitaPromovida Ponteiro para armazenar o RRN do novo nó filho à direita.
 * @param proxRRNCabec Ponteiro para atualizar o próximo RRN disponível para uso.
 * @return Retorna um valor indicando o estado da inserção (promoção, sem promoção, erro, etc.).
 */
int inserir(FILE *arqIndice, int RRNAtual, long chave, long referencia,
            long *chavePromovida, long *referenciaPromovida, int *filhoDireitaPromovida, int *proxRRNCabec);

/**
 * Função para realizar a divisão de um nó na árvore-B durante a inserção (split).
 * Divide um nó cheio em dois e promove uma chave para o nó pai, mantendo a estrutura da árvore.
 * @param arqIndice Ponteiro para o arquivo de índice onde a árvore-B é armazenada.
 * @param chaveInserir Chave que será inserida no nó.
 * @param referenciaInserir Referência associada à chave a ser inserida.
 * @param rrnFilhoDireita RRN do nó filho à direita da chave a ser inserida.
 * @param pagina Ponteiro para o nó (página) atual da árvore-B.
 * @param chavePromovida Ponteiro para armazenar a chave promovida após a divisão.
 * @param referenciaPromovida Ponteiro para armazenar a referência promovida após a divisão.
 * @param filhoDireitaPromovida Ponteiro para armazenar o RRN do novo nó filho à direita.
 * @param novaPagina Ponteiro para o novo nó criado como resultado da divisão.
 * @param proxRRNCabec Ponteiro para atualizar o próximo RRN disponível para uso.
 */
void split(FILE *arqIndice, long chaveInserir, long referenciaInserir, int rrnFilhoDireita,
           Pagina *pagina, long *chavePromovida, long *referenciaPromovida,
           int *filhoDireitaPromovida, Pagina *novaPagina, int *proxRRNCabec);

#endif // FUNCOES_ARVORE_B_H
