#include "stdio.h"
#include "stdlib.h"
#include "funcoesFornecidas.h"
#include "cabecalhos.h"
#include "registros.h"
#include "funcoesArvoreB.h"



long busca(FILE *arqIndice, int RRNAtual, long chave) {
    // Verifica se o RRNAtual é -1, indicando um nó inexistente ou folha final, e retorna um valor de "não encontrado".
    if (RRNAtual == -1) return NAO_ENCONTRADO;

    // Posiciona o ponteiro de leitura no arquivo para a posição do nó desejado.
    // Calcula o deslocamento com base no RRNAtual e o tamanho de uma página.
    fseek(arqIndice, OFFSET_CABECALHO_ARVB + RRNAtual * TAM_PAGINA, SEEK_SET);

    // Lê o conteúdo da página no arquivo e carrega para a estrutura Pagina.
    Pagina *paginaAtual = lePaginaBin(arqIndice);

    int pos;
    // Realiza uma busca sequencial nas chaves do nó atual para encontrar a posição onde a chave buscada pode estar.
    // A iteração ocorre enquanto:
    // - `pos` é menor que a ordem - 1 (para evitar overflow de chaves),
    // - `pos` é menor que o número de chaves indexadas no nó (não ultrapassa o limite de chaves existentes),
    // - e a chave atual na posição `pos` é menor que a chave desejada.
    for (pos = 0; pos < ORDEM - 1 && pos < paginaAtual->nroChavesIndexadas && chave > paginaAtual->chaves[pos]; pos++);

    // Verifica se a chave na posição `pos` é a chave buscada:
    // - Confirma que `pos` está dentro do número de chaves da ordem.
    // - Se a chave na posição `pos` for igual à chave buscada, a referência associada a essa chave é retornada.
    if (pos < ORDEM - 1 && chave == paginaAtual->chaves[pos]) {
        long referencia = paginaAtual->referencias[pos];
        free(paginaAtual); // Libera a memória da página lida
        return referencia;  // Retorna a referência encontrada
    } else {
        // Caso a chave não esteja no nó atual, a função `busca` é chamada recursivamente no nó filho apropriado.
        // O ponteiro na posição `pos` é usado para acessar o próximo nó que possivelmente contém a chave.
        // Essa abordagem realiza uma busca recursiva, navegando pela árvore até encontrar a chave ou alcançar uma folha final.
        int resultado = busca(arqIndice, paginaAtual->ponteiros[pos], chave);
        free(paginaAtual); // Libera a memória da página lida após a busca recursiva
        return resultado;  // Retorna o resultado da busca
    }
}
    



void split(FILE *arqIndice, long chaveInserir, long referenciaInserir, int rrnFilhoDireita,
           Pagina *pagina, long *chavePromovida, long *referenciaPromovida,
           int *filhoDireitaPromovida, Pagina *novaPagina, int *proxRRNCabec) {

    // Vetores temporários para armazenar chaves, referências e ponteiros da página
    long chaves[ORDEM];
    long referencias[ORDEM];
    int ponteiros[ORDEM + 1];

    int i, posInserir;

    // Encontra a posição onde a nova chave será inserida.
    // Percorre as chaves existentes e para quando encontra uma chave maior ou igual a chaveInserir.
    for (posInserir = 0; posInserir < pagina->nroChavesIndexadas && pagina->chaves[posInserir] < chaveInserir; posInserir++);

    // Copia as chaves e referências da página atual para os vetores temporários.
    for (i = 0; i < pagina->nroChavesIndexadas; i++) {
        chaves[i] = pagina->chaves[i];
        referencias[i] = pagina->referencias[i];
    }

    // Copia os ponteiros da página atual para o vetor temporário de ponteiros.
    for (i = 0; i <= pagina->nroChavesIndexadas; i++) {
        ponteiros[i] = pagina->ponteiros[i];
    }

    // Desloca as chaves e referências nos vetores temporários para dar espaço à nova chave.
    for (i = pagina->nroChavesIndexadas; i > posInserir; i--) {
        chaves[i] = chaves[i - 1];
        referencias[i] = referencias[i - 1];
    }

    // Insere a nova chave e referência na posição correta.
    chaves[posInserir] = chaveInserir;
    referencias[posInserir] = referenciaInserir;

    // Desloca os ponteiros nos vetores temporários para dar espaço ao novo ponteiro.
    for (i = pagina->nroChavesIndexadas + 1; i > posInserir + 1; i--) {
        ponteiros[i] = ponteiros[i - 1];
    }
    // Insere o novo ponteiro na posição adequada.
    ponteiros[posInserir + 1] = rrnFilhoDireita;

    // Calcula o total de chaves após a inserção.
    int totalChaves = pagina->nroChavesIndexadas + 1;

    // Define o ponto de promoção, que é o valor da chave no meio dos vetores temporários.
    int meio = 2;
    *chavePromovida = chaves[meio];
    *referenciaPromovida = referencias[meio];

    // Atualiza a página original para manter as chaves à esquerda do ponto de promoção.
    pagina->nroChavesIndexadas = meio;
    for (i = 0; i < meio; i++) {
        pagina->chaves[i] = chaves[i];
        pagina->referencias[i] = referencias[i];
        pagina->ponteiros[i] = ponteiros[i];
    }
    // O último ponteiro da página original deve apontar para o primeiro nó à direita das chaves.
    pagina->ponteiros[i] = ponteiros[i];

    // Limpa os valores remanescentes na página original.
    for (i = meio; i < ORDEM - 1; i++) {
        pagina->chaves[i] = -1;
        pagina->referencias[i] = -1;
        pagina->ponteiros[i + 1] = -1;
    }

    // Preenche a nova página (à direita do ponto de promoção) com as chaves e ponteiros restantes.
    novaPagina->nroChavesIndexadas = 2;
    novaPagina->folha = pagina->folha;
    for (i = 0; i < novaPagina->nroChavesIndexadas; i++) {
        novaPagina->chaves[i] = chaves[meio + 1 + i];
        novaPagina->referencias[i] = referencias[meio + 1 + i];
        novaPagina->ponteiros[i] = ponteiros[meio + 1 + i];
    }
    // O último ponteiro da nova página deve apontar para o próximo nó à direita.
    novaPagina->ponteiros[i] = ponteiros[meio + 1 + i];

    // Atribui um novo RRN para a nova página e atualiza o próximo RRN disponível no cabeçalho.
    novaPagina->RRNdoNo = (*proxRRNCabec)++;
    *filhoDireitaPromovida = novaPagina->RRNdoNo;
}


int inserir(FILE *arqIndice, int RRNAtual, long chave, long referencia,
            long *chavePromovida, long *referenciaPromovida, int *filhoDireitaPromovida, int *proxRRNCabec) {
    
    // Caso base: se a posição atual é -1, retorna promoção para inserir a chave no nó acima
    if (RRNAtual == -1) {
        *chavePromovida = chave;
        *referenciaPromovida = referencia;
        *filhoDireitaPromovida = -1;
        return PROMOCAO;
    } else {

        // Armazena a chave promovida do nível inferior, caso ocorra um split
        long chavePromovidaNivelInf;

        // Armazena a referência associada à chave promovida do nível inferior, usada para acesso rápido ao dado
        long referenciaPromovidaNivelInf;

        // Armazena o RRN do nó filho à direita promovido, resultante de um split em níveis inferiores
        int RRNPromovidoInf;

        // Carrega a página atual para memória a partir do arquivo de índice
        fseek(arqIndice, OFFSET_CABECALHO_ARVB + RRNAtual * TAM_PAGINA, SEEK_SET);
        Pagina *paginaAtual = lePaginaBin(arqIndice);

        // Encontra a posição correta de inserção
        int pos;
        for (pos = 0; pos < paginaAtual->nroChavesIndexadas && chave > paginaAtual->chaves[pos]; pos++);

        // Se a chave já existe, retorna erro
        if (pos < paginaAtual->nroChavesIndexadas && chave == paginaAtual->chaves[pos]) {
            free(paginaAtual);
            return ERRO;
        }

        // Recursão para inserir a chave no próximo nó (filho) encontrado
        int retorno = inserir(arqIndice, paginaAtual->ponteiros[pos], chave, referencia,
                              &chavePromovidaNivelInf, &referenciaPromovidaNivelInf, &RRNPromovidoInf, proxRRNCabec);

        // Caso não haja promoção, ou se ocorreu um erro, apenas retorna
        if (retorno == SEM_PROMOCAO || retorno == ERRO) {
            free(paginaAtual);
            return retorno;
        }

        // Caso o nó atual ainda tenha espaço, insere a nova chave promovida ordenadamente
        if (paginaAtual->nroChavesIndexadas < ORDEM - 1) {
            // Desloca chaves e referências para dar espaço para a nova chave
            for (int i = paginaAtual->nroChavesIndexadas; i > pos; i--) {
                paginaAtual->chaves[i] = paginaAtual->chaves[i - 1];
                paginaAtual->referencias[i] = paginaAtual->referencias[i - 1];
                paginaAtual->ponteiros[i + 1] = paginaAtual->ponteiros[i];
            }
            // Insere a chave e referência na posição correta para manter a ordem
            paginaAtual->chaves[pos] = chavePromovidaNivelInf;
            paginaAtual->referencias[pos] = referenciaPromovidaNivelInf;
            paginaAtual->ponteiros[pos + 1] = RRNPromovidoInf;
            (paginaAtual->nroChavesIndexadas)++;

            // Grava a página atual de volta no arquivo de índice
            fseek(arqIndice, OFFSET_CABECALHO_ARVB + RRNAtual * TAM_PAGINA, SEEK_SET);
            escrevePaginaBin(arqIndice, paginaAtual);

            free(paginaAtual);
            return SEM_PROMOCAO;
        } else {
            // Se o nó está cheio, realiza um split
            Pagina *novaPagina = criaPagina();
            split(arqIndice, chavePromovidaNivelInf, referenciaPromovidaNivelInf, RRNPromovidoInf,
                  paginaAtual, chavePromovida, referenciaPromovida, filhoDireitaPromovida, novaPagina, proxRRNCabec);

            // Grava a página atual e a nova página resultante do split
            fseek(arqIndice, OFFSET_CABECALHO_ARVB + RRNAtual * TAM_PAGINA, SEEK_SET);
            escrevePaginaBin(arqIndice, paginaAtual);

            fseek(arqIndice, OFFSET_CABECALHO_ARVB + novaPagina->RRNdoNo * TAM_PAGINA, SEEK_SET);
            escrevePaginaBin(arqIndice, novaPagina);

            free(paginaAtual);
            free(novaPagina);

            return PROMOCAO;
        }
    }
}
