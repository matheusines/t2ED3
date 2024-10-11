#include "stdio.h"
#include "stdlib.h"
#include "funcoesFornecidas.h"
#include "cabecalho.h"
#include "registro.h"
#include "funcoesArvoreB.h"





RegistroArvoreB *criaPagina() {
    RegistroArvoreB *novaPagina = (RegistroArvoreB *)malloc(sizeof(RegistroArvoreB));
    novaPagina->nroChavesIndexadas = 0;
    novaPagina->folha = '1'; // Assign '1' as a character to indicate a leaf node
    novaPagina->RRNdoNo = -1;

    for (int i = 0; i < ORDEM; i++) {
        novaPagina->ponteiros[i] = -1;
    }

    for (int i = 0; i < ORDEM - 1; i++) {
        novaPagina->chaves[i] = -1;
        novaPagina->referencias[i] = -1;
    }

    return novaPagina;
}


void split(FILE *arqIndice, long chaveInserir, long referenciaInserir, int rrnFilhoDireita,
           RegistroArvoreB *pagina, long *chavePromovida, long *referenciaPromovida,
           int *filhoDireitaPromovida, RegistroArvoreB *novaPagina, int *proxRRNCabec) {
    long chaves[ORDEM];
    long referencias[ORDEM];
    int ponteiros[ORDEM + 1];


    int i, posInserir;

    // Find the position to insert the new key
    for (posInserir = 0; posInserir < (pagina->nroChavesIndexadas) && pagina->chaves[posInserir] < chaveInserir; posInserir++);

    // Copy existing keys and pointers to temporary arrays
    for (i = 0; i < posInserir; i++) {
        chaves[i] = pagina->chaves[i];
        referencias[i] = pagina->referencias[i];
        ponteiros[i] = pagina->ponteiros[i];
    }
    ponteiros[i] = pagina->ponteiros[i];

    // Insert the new key and pointer
    chaves[posInserir] = chaveInserir;
    referencias[posInserir] = referenciaInserir;
    ponteiros[posInserir + 1] = rrnFilhoDireita;

    // Copy the remaining keys and pointers
    for (i = posInserir; i < pagina->nroChavesIndexadas; i++) {
        chaves[i + 1] = pagina->chaves[i];
        referencias[i + 1] = pagina->referencias[i];
        ponteiros[i + 2] = pagina->ponteiros[i + 1];
    }

    // Calculate the median index
    int meio = ORDEM / 2;

    // Promote the median key
    *chavePromovida = chaves[meio];
    *referenciaPromovida = referencias[meio];

    // Left node (original page)
    pagina->nroChavesIndexadas = meio;
    for (i = 0; i < meio; i++) {
        pagina->chaves[i] = chaves[i];
        pagina->referencias[i] = referencias[i];
        pagina->ponteiros[i] = ponteiros[i];
    }
    pagina->ponteiros[i] = ponteiros[i];

    // Clean up the rest of the original page
    for (i = meio; i < ORDEM - 1; i++) {
        pagina->chaves[i] = -1;
        pagina->referencias[i] = -1;
        pagina->ponteiros[i + 1] = -1;
    }

    // Right node (new page)
    novaPagina->nroChavesIndexadas = ORDEM - meio - 1;
    novaPagina->folha = pagina->folha;
    for (i = 0; i < novaPagina->nroChavesIndexadas; i++) {
        novaPagina->chaves[i] = chaves[meio + 1 + i];
        novaPagina->referencias[i] = referencias[meio + 1 + i];
        novaPagina->ponteiros[i] = ponteiros[meio + 1 + i];
    }
    novaPagina->ponteiros[novaPagina->nroChavesIndexadas] = ponteiros[ORDEM];

    // Assign the RRN of the new page
    novaPagina->RRNdoNo = (*proxRRNCabec)++;
    *filhoDireitaPromovida = novaPagina->RRNdoNo;
}



void escrevePaginaBin(FILE *fp, RegistroArvoreB *pag) {
    fwrite(&pag->folha, sizeof(char), 1, fp);
    fwrite(&pag->nroChavesIndexadas, sizeof(int), 1, fp);
    fwrite(&pag->RRNdoNo, sizeof(int), 1, fp);
    
    int i;
    for( i = 0; i < ORDEM - 1; i++) {
        fwrite(&pag->ponteiros[i], sizeof(int), 1, fp);
        fwrite(&pag->chaves[i], sizeof(long), 1, fp);
        fwrite(&pag->referencias[i], sizeof(long), 1, fp);
        //fwrite(&pag->ponteiros[i], sizeof(int), 1, fp);
    }
    fwrite(&pag->ponteiros[i], sizeof(int), 1, fp);
    
}

int busca(FILE *arqIndice, int RRNAtual, long chave) {
    if (RRNAtual == -1) return NAO_ENCONTRADO;

    fseek(arqIndice, OFFSET_CABECALHO_ARVB + RRNAtual * TAM_PAGINA, SEEK_SET);
    RegistroArvoreB *paginaAtual = lePaginaBin(arqIndice);

    int pos;
    for (pos = 0; pos < paginaAtual->nroChavesIndexadas && chave > paginaAtual->chaves[pos]; pos++);

    if (pos < paginaAtual->nroChavesIndexadas && chave == paginaAtual->chaves[pos]) {
        long referencia = paginaAtual->referencias[pos];
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
    int i;
    for (i = 0; i < ORDEM - 1; i++) {
        fread(&pag->ponteiros[i], sizeof(int), 1, fp);
        fread(&pag->chaves[i], sizeof(long), 1, fp);
        fread(&pag->referencias[i], sizeof(long), 1, fp);
        
    }

    fread(&pag->ponteiros[i], sizeof(int), 1, fp);
    

    return pag;
}

int inserir(FILE *arqIndice, int RRNAtual, long chave, long referencia,
            long *chavePromovida, long *referenciaPromovida, int *filhoDireitaPromovida, int *proxRRNCabec) {
    if (RRNAtual == -1) {
        // Tree is empty or reached a leaf; promote the key
        *chavePromovida = chave;
        *referenciaPromovida = referencia;
        *filhoDireitaPromovida = -1;
        return PROMOCAO;
    } else {
        // Read the current page
        fseek(arqIndice, OFFSET_CABECALHO_ARVB + RRNAtual * TAM_PAGINA, SEEK_SET);
        RegistroArvoreB *paginaAtual = lePaginaBin(arqIndice);

        // Search for the position to insert
        int pos;
        for (pos = 0; pos < paginaAtual->nroChavesIndexadas && chave > paginaAtual->chaves[pos]; pos++);

        // Check for duplicate key
        if (pos < paginaAtual->nroChavesIndexadas && chave == paginaAtual->chaves[pos]) {
            free(paginaAtual);
            return ERRO; // Key already exists
        }

        // Recursive insertion
        int retorno = inserir(arqIndice, paginaAtual->ponteiros[pos], chave, referencia,
                              chavePromovida, referenciaPromovida, filhoDireitaPromovida, proxRRNCabec);

        if (retorno == SEM_PROMOCAO || retorno == ERRO) {
            free(paginaAtual);
            return retorno;
        }

        // Insert the promoted key into the current page
        if (paginaAtual->nroChavesIndexadas < ORDEM - 1) {
            // Shift keys and pointers to make room
            for (int i = paginaAtual->nroChavesIndexadas; i > pos; i--) {
                paginaAtual->chaves[i] = paginaAtual->chaves[i - 1];
                paginaAtual->referencias[i] = paginaAtual->referencias[i - 1];
                paginaAtual->ponteiros[i + 1] = paginaAtual->ponteiros[i];
            }
            // Insert the promoted key
            paginaAtual->chaves[pos] = *chavePromovida;
            paginaAtual->referencias[pos] = *referenciaPromovida;
            paginaAtual->ponteiros[pos + 1] = *filhoDireitaPromovida;
            paginaAtual->nroChavesIndexadas++;

            // Write the updated page back to the file
            fseek(arqIndice, OFFSET_CABECALHO_ARVB + RRNAtual * TAM_PAGINA, SEEK_SET);
            escrevePaginaBin(arqIndice, paginaAtual);

            free(paginaAtual);
            return SEM_PROMOCAO;
        } else {
            // Need to split the page
            RegistroArvoreB *novaPagina = criaPagina();

            split(arqIndice, *chavePromovida, *referenciaPromovida, *filhoDireitaPromovida,
                  paginaAtual, chavePromovida, referenciaPromovida, filhoDireitaPromovida, novaPagina, proxRRNCabec);

            // Write the updated current page
            fseek(arqIndice, OFFSET_CABECALHO_ARVB + RRNAtual * TAM_PAGINA, SEEK_SET);
            escrevePaginaBin(arqIndice, paginaAtual);

            // Write the new page
            fseek(arqIndice, OFFSET_CABECALHO_ARVB + novaPagina->RRNdoNo * TAM_PAGINA, SEEK_SET);
            escrevePaginaBin(arqIndice, novaPagina);

            free(paginaAtual);
            free(novaPagina);

            return PROMOCAO;
        }
    }
}
