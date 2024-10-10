
#ifndef FUNCOES_ARVORE_B_H
#define FUNCOES_ARVORE_B_H

#include <stdio.h>
#include <stdlib.h>
#include "registro.h"
#include "cabecalho.h"


RegistroArvoreB *criaPagina();

void escrevePaginaBin(FILE *fp, RegistroArvoreB *pag);

int busca(FILE *arqIndice, int RRNAtual, int chave);

int inserir(FILE *arqIndice, int RRNAtual, int chave, int referencia, int *chavePromovida, int *referenciaPromovida, int *filhoDireitoPromovida, int *proxRRNCabec);

void split(FILE *arqIndice, int chaveInserir, int referenciaInserir, int rrnFilhoDireita, RegistroArvoreB *pagina, int *chavePromovida, int *referenciaPromovida, int *filhoDireitaPromovida, RegistroArvoreB *novaPagina);

RegistroArvoreB *lePaginaBin(FILE *fp);

#endif


