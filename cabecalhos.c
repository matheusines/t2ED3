#include <ctype.h>            
#include <string.h>           
#include "funcoesFornecidas.h" 
#include "cabecalhos.h"        

#define DELIMITER '#'         // Define um delimitador, que pode ser usado em outra parte do código
#define TRASH '$'             // Define o caractere de lixo, utilizado para preencher o espaço vazio no cabeçalho
#define PAGE_SIZE_T1 1600        // Define o tamanho da página de disco, com 1600 bytes

// Função para inicializar os campos do cabeçalho com valores padrão
Cabecalho inicializarCabecalho() {
    Cabecalho header;          // Criação da estrutura de cabeçalho
    header.status = '0';       // Status '0' indica que o arquivo está inconsistente
    header.topo = -1;          // O topo da lista de registros removidos é inicializado como -1 (nenhum registro removido)
    header.proxRRN = 0;        // O próximo RRN é 0, já que ainda não há registros
    header.nroRegRem = 0;      // Nenhum registro removido inicialmente
    header.nroPagDisco = 1;    // Inicialmente, existe apenas uma página de disco (o cabeçalho)
    header.qttCompacta = 0;    // Nenhuma compactação foi realizada até o momento
    return header;             // Retorna o cabeçalho inicializado
}

// Função para escrever o cabeçalho em um arquivo binário
void escreverCabecalhoBin(FILE *fp, Cabecalho *header) {
    if (fp == NULL) return;    // Verifica se o ponteiro do arquivo é válido

    // Escreve os campos do cabeçalho no arquivo binário
    fwrite(&header->status, sizeof(char), 1, fp);         
    fwrite(&header->topo, sizeof(int), 1, fp);            
    fwrite(&header->proxRRN, sizeof(int), 1, fp);         
    fwrite(&header->nroRegRem, sizeof(int), 1, fp);       
    fwrite(&header->nroPagDisco, sizeof(int), 1, fp);     
    fwrite(&header->qttCompacta, sizeof(int), 1, fp);     

    // Preenche o restante da página de 1600 bytes com lixo (caractere '$')
    char lixo = TRASH;
    for (int i = 0; i < 1579; i++) {   // 1600 - tamanho dos campos já escritos = 1579 bytes de lixo
        fwrite(&lixo, sizeof(char), 1, fp);
    }
}


// Função para ler o cabeçalho de um arquivo binário e ignorar o lixo
void lerCabecalhoBin(FILE *fp, Cabecalho *header) {
    if (fp == NULL || header == NULL) return; // Verifica se o arquivo ou o cabeçalho são válidos

    // Lê os campos do cabeçalho a partir do arquivo binário
    fread(&header->status, sizeof(char), 1, fp);          
    fread(&header->topo, sizeof(int), 1, fp);             
    fread(&header->proxRRN, sizeof(int), 1, fp);          
    fread(&header->nroRegRem, sizeof(int), 1, fp);        
    fread(&header->nroPagDisco, sizeof(int), 1, fp);      
    fread(&header->qttCompacta, sizeof(int), 1, fp);      
    
    // Calcula a posição atual do arquivo e pula os bytes de lixo restantes
    long int currentPos = ftell(fp);                      // Obtém a posição atual no arquivo
    long int endOfHeaderPage = PAGE_SIZE_T1;                 // Define o final da página (1600 bytes)
    fseek(fp, endOfHeaderPage - currentPos, SEEK_CUR);    // Move o ponteiro do arquivo para pular o lixo
}

/*

**************************************************************************************************************************************
*                                                                                                                                    *
*                                                             TRABALHO 2                                                             *
*                                                                                                                                    *
**************************************************************************************************************************************

*/



CabecalhoIndice inicializarCabecalhoIndice() {
    CabecalhoIndice headerArvoreB;          // Criação da estrutura de cabeçalho do arquivo de indice
    headerArvoreB.status = '0';       // Status '0' indica que o arquivo está inconsistente
    headerArvoreB.noRaiz = -1;      //Como não há nós no novo cabeçalho, o nó raiz recebe -1
    headerArvoreB.RRNproxNo = 0;    //RRN do primeiro nó a ser inserido
    return headerArvoreB;             // Retorna o cabeçalho inicializado
}

// Função para escrever o cabeçalho indice em um arquivo binário
void escreverCabecalhoIndice(FILE *fp, CabecalhoIndice *header) {
    if (fp == NULL) return;    // Verifica se o ponteiro do arquivo é válido

    // Escreve os campos do cabeçalho no arquivo binário
    fwrite(&header->status, sizeof(char), 1, fp);         
    fwrite(&header->noRaiz, sizeof(int), 1, fp);
    fwrite(&header->RRNproxNo, sizeof(int), 1, fp);            

    // Preenche o restante da página de 84 bytes com lixo (caractere '$')
    char lixo = TRASH;
    for (int i = 0; i < 84; i++) {   // 93 - tamanho dos campos já escritos = 84 bytes de lixo
        fwrite(&lixo, sizeof(char), 1, fp);
    }
}

void lerCabecalhoIndice(FILE *fp, CabecalhoIndice *cabecalho) {
    if (fp == NULL) return; // erro

    // Lê os campos do cabeçalho
    fread(&cabecalho->status, sizeof(char), 1, fp);
    fread(&cabecalho->noRaiz, sizeof(int), 1, fp);
    fread(&cabecalho->RRNproxNo, sizeof(int), 1, fp);

    // Avança o ponteiro para o próximo campo, caso haja espaço de preenchimento no cabeçalho
    fseek(fp, OFFSET_CABECALHO_ARVB, SEEK_SET);
}
