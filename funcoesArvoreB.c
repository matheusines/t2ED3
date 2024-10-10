#include "stdio.h"
#include "stdlib.h"
#include "funcoesFornecidas.h"
#include "cabecalho.h"
#include "registro.h"
#include "funcoesArvoreB.h"





RegistroArvoreB *criaPagina() {
    RegistroArvoreB *novaPagina = (RegistroArvoreB *) malloc(sizeof(RegistroArvoreB));
    novaPagina->nroChavesIndexadas = 0;
    novaPagina->folha = 1;
    novaPagina->RRNdoNo = -1;

    for(int i = 0; i < ORDEM - 1; i++) {
        novaPagina->chaves[i] = -1;
        novaPagina->referencias[i] = -1;
    }

    for(int i = 0; i < ORDEM; i++) {
        novaPagina->ponteiros[i] = -1;
    }

    return novaPagina;
}

void split(FILE *arqIndice, int chaveInserir, int referenciaInserir, int rrnFilhoDireita, RegistroArvoreB *pagina, int *chavePromovida, int *referenciaPromovida, int *filhoDireitaPromovida, RegistroArvoreB *novaPagina) {
    int chaves[ORDEM];
    int referencias[ORDEM];
    int ponteiros[ORDEM + 1];

    int i, posInserir;

    // Localiza a posição onde a nova chave deve ser inserida
    for (posInserir = 0; posInserir < pagina->nroChavesIndexadas && pagina->chaves[posInserir] < chaveInserir; posInserir++);

    // Copia os valores atuais para os vetores auxiliares
    for (i = 0; i < posInserir; i++) {
        chaves[i] = pagina->chaves[i];
        referencias[i] = pagina->referencias[i];
        ponteiros[i] = pagina->ponteiros[i];
    }
    ponteiros[i] = pagina->ponteiros[i];

    // Insere a nova chave no vetor auxiliar na posição correta
    chaves[posInserir] = chaveInserir;
    referencias[posInserir] = referenciaInserir;
    ponteiros[posInserir + 1] = rrnFilhoDireita;

    for (i = posInserir; i < pagina->nroChavesIndexadas; i++) {
        chaves[i + 1] = pagina->chaves[i];
        referencias[i + 1] = pagina->referencias[i];
        ponteiros[i + 2] = pagina->ponteiros[i + 1];
    }

    int posPromo = ORDEM / 2;

    *chavePromovida = chaves[posPromo];
    *referenciaPromovida = referencias[posPromo];

    // Atualiza a página atual com a primeira metade dos elementos
    for (i = 0; i < posPromo; i++) {
        pagina->chaves[i] = chaves[i];
        pagina->referencias[i] = referencias[i];
        pagina->ponteiros[i] = ponteiros[i];
    }
    pagina->ponteiros[i] = ponteiros[i];
    
    for (i = posPromo; i < ORDEM - 1; i++) {
        pagina->chaves[i] = -1;
        pagina->referencias[i] = -1;
        pagina->ponteiros[i + 1] = -1;
    }

    pagina->nroChavesIndexadas = posPromo;

    // Prepara a nova página para armazenar a segunda metade dos elementos
    for (i = 0; i < ORDEM - posPromo - 1; i++) {
        novaPagina->chaves[i] = chaves[posPromo + 1 + i];
        novaPagina->referencias[i] = referencias[posPromo + 1 + i];
        novaPagina->ponteiros[i] = ponteiros[posPromo + 1 + i];
    }
    novaPagina->ponteiros[i] = ponteiros[posPromo + 1 + i];

    novaPagina->nroChavesIndexadas = ORDEM - posPromo - 1;
    novaPagina->folha = pagina->folha;
    novaPagina->RRNdoNo = *filhoDireitaPromovida;

    *filhoDireitaPromovida = novaPagina->RRNdoNo;

    fseek(arqIndice, 0, SEEK_END);
    escrevePaginaBin(arqIndice, novaPagina);
}

void escrevePaginaBin(FILE *fp, RegistroArvoreB *pag) {
    fwrite(&pag->folha, sizeof(char), 1, fp);
    fwrite(&pag->nroChavesIndexadas, sizeof(int), 1, fp);
    fwrite(&pag->RRNdoNo, sizeof(int), 1, fp);

    for(int i = 0; i < ORDEM - 1; i++) {
        fwrite(&pag->chaves[i], sizeof(int), 1, fp);
        fwrite(&pag->referencias[i], sizeof(int), 1, fp);
    }

    for(int i = 0; i < ORDEM; i++) {
        fwrite(&pag->ponteiros[i], sizeof(int), 1, fp);
    }
}

int busca(FILE *arqIndice, int RRNAtual, int chave) {
    if(RRNAtual == -1) return NAO_ENCONTRADO;
    
    fseek(arqIndice, OFFSET_CABECALHO_ARVB + (RRNAtual * TAM_PAGINA), SEEK_SET);
    RegistroArvoreB *paginaAtual = lePaginaBin(arqIndice);

    int pos;
    for(pos = 0; pos < paginaAtual->nroChavesIndexadas && paginaAtual->chaves[pos] < chave; pos++);

    if(pos < paginaAtual->nroChavesIndexadas && paginaAtual->chaves[pos] == chave) {
        int referencia = paginaAtual->referencias[pos];
        free(paginaAtual);
        return referencia;
    } else {
        int resultado = busca(arqIndice, paginaAtual->ponteiros[pos], chave);
        free(paginaAtual);
        return resultado;
    }
}

RegistroArvoreB *lePaginaBin(FILE *fp) {
    if (fp == NULL) return NULL; // erro

    RegistroArvoreB *pag = criaPagina();

    // erro, nao ha mais o que ler
    if (fread(&pag->folha, sizeof(char), 1, fp) == 0) {
        free(pag);
        return NULL;
    }
    
    fread(&pag->nroChavesIndexadas, sizeof(int), 1, fp);
    fread(&pag->RRNdoNo, sizeof(int), 1, fp);

    for (int i = 0; i < ORDEM - 1; i++) {
        fread(&pag->chaves[i], sizeof(int), 1, fp);
        fread(&pag->referencias[i], sizeof(int), 1, fp);
    }

    for (int i = 0; i < ORDEM; i++) {
        fread(&pag->ponteiros[i], sizeof(int), 1, fp);
    }

    return pag;
}

int inserir(FILE *arqIndice, int RRNAtual, int chave, int referencia, int *chavePromovida, int *referenciaPromovida, int *filhoDireitaPromovida, int *proxRRNCabec) {
    if(RRNAtual == -1) {
        *chavePromovida = chave;
        *referenciaPromovida = referencia;
        *filhoDireitaPromovida = -1;
        return PROMOCAO;
    } else {
        fseek(arqIndice, OFFSET_CABECALHO_ARVB + (RRNAtual * TAM_PAGINA), SEEK_SET);
        RegistroArvoreB *paginaAtual = lePaginaBin(arqIndice);

        int pos;
        for(pos = 0; pos < paginaAtual->nroChavesIndexadas && paginaAtual->chaves[pos] < chave; pos++);
        
        if(chave == paginaAtual->chaves[pos]) {
            free(paginaAtual);
            return ERRO;
        }

        int valorRetorno = inserir(arqIndice, paginaAtual->ponteiros[pos], chave, referencia, chavePromovida, referenciaPromovida, filhoDireitaPromovida, proxRRNCabec);
        
        if(valorRetorno == SEM_PROMOCAO || valorRetorno == ERRO) {
            free(paginaAtual);
            return valorRetorno;
        } else if(paginaAtual->nroChavesIndexadas < ORDEM - 1) {
            for(int i = paginaAtual->nroChavesIndexadas; i > pos; i--) {
                paginaAtual->chaves[i] = paginaAtual->chaves[i - 1];
                paginaAtual->referencias[i] = paginaAtual->referencias[i - 1];
                paginaAtual->ponteiros[i + 1] = paginaAtual->ponteiros[i];
            }

            paginaAtual->chaves[pos] = *chavePromovida;
            paginaAtual->referencias[pos] = *referenciaPromovida;
            paginaAtual->ponteiros[pos + 1] = *filhoDireitaPromovida;

            (paginaAtual->nroChavesIndexadas)++;
            fseek(arqIndice, OFFSET_CABECALHO_ARVB + (RRNAtual * TAM_PAGINA), SEEK_SET);
            escrevePaginaBin(arqIndice, paginaAtual);

            free(paginaAtual);
            return SEM_PROMOCAO;
        } else {
            RegistroArvoreB *novaPagina = criaPagina();
            (*proxRRNCabec)++;
            split(arqIndice, *chavePromovida, *referenciaPromovida, *filhoDireitaPromovida, paginaAtual, chavePromovida, referenciaPromovida, filhoDireitaPromovida, novaPagina);

            fseek(arqIndice, OFFSET_CABECALHO_ARVB + (RRNAtual * TAM_PAGINA), SEEK_SET);
            escrevePaginaBin(arqIndice, paginaAtual);

            fseek(arqIndice, OFFSET_CABECALHO_ARVB + ((*filhoDireitaPromovida) * TAM_PAGINA), SEEK_SET);
            escrevePaginaBin(arqIndice, novaPagina);

            free(paginaAtual);
            free(novaPagina);

            return PROMOCAO;
        }
    }
}