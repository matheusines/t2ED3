
#ifndef FUNCOES_ARVORE_B_H
#define FUNCOES_ARVORE_B_H

#include <stdio.h>
#include <stdlib.h>
#include "registro.h"
#include "cabecalho.h"


RegistroArvoreB *criaPagina();

void escrevePaginaBin(FILE *fp, RegistroArvoreB *pag);

int busca(FILE *arqIndice, int RRNAtual, long chave);

int inserir(FILE *arqIndice, int RRNAtual, long chave, long referencia,
            long *chavePromovida, long *referenciaPromovida, int *filhoDireitaPromovida, int *proxRRNCabec);

void split(FILE *arqIndice, long chaveInserir, long referenciaInserir, int rrnFilhoDireita,
           RegistroArvoreB *pagina, long *chavePromovida, long *referenciaPromovida,
           int *filhoDireitaPromovida, RegistroArvoreB *novaPagina, int *proxRRNCabec);

RegistroArvoreB *lePaginaBin(FILE *fp);

#endif


