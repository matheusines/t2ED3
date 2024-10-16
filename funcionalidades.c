#include <ctype.h>
#include <string.h>
#include <math.h>
#include "funcoesFornecidas.h"
#include "funcoesArvoreB.h"

#define DELIMITER '#'
#define TRASH '$'
#define PAGE_SIZE_T1 1600

//--------------------------------------------------------------- FUNCIONALIDADE 1 (CREATE TABLE) ---------------------------------------------------------------

void funcionalidade1() {
    Cabecalho header = inicializarCabecalho();  // Inicializa o cabeçalho para armazenar metadados do arquivo binário

    char nomeArqCSV[30], nomeArqBin[30];  
    scanf("%s %s", nomeArqCSV, nomeArqBin);  // Lê os nomes dos arquivos CSV e binário

    // Tenta abrir o arquivo CSV para leitura
    FILE *arquivoCSV = fopen(nomeArqCSV, "r");
    if (arquivoCSV == NULL) {
        // Se o arquivo CSV não puder ser aberto, exibe a mensagem de erro e termina a função
        printf("Falha no processamento do arquivo.");
        return;
    }

    // Abre o arquivo binário para escrita no modo binário (wb+)
    FILE *arquivoBin = fopen(nomeArqBin, "wb+");

    // Escreve o cabeçalho no início do arquivo binário
    escreverCabecalhoBin(arquivoBin, &header);

    // Ignora a primeira linha do arquivo CSV, pois contém os titulos das colunas
    fscanf(arquivoCSV, "%*[^\n]\n");

    Registro *r = criarRegistro();  // Inicializa o registro que será usado
    r = lerRegistroCSV(arquivoCSV);  // Lê o primeiro registro do CSV

    int contPags = 1;  // Inicializa o contador de páginas de disco (começa com 1 devido ao cabeçalho)
    int numBytes = 0;  // Controla o número de bytes escritos no arquivo para saber quando atingir o limite de uma página de 1600 bytes

    // Loop que percorre o arquivo CSV até o final
    while (r != NULL) {
        escreverRegistro(arquivoBin, r);  // Escreve o registro que foi lido do CSV no arquivo binário
        liberarRegistro(r);  // Libera o espaço de memória ocupado pelo registro após escrevê-lo
        r = lerRegistroCSV(arquivoCSV);  // Lê o próximo registro do arquivo CSV
        header.proxRRN++;  // Atualiza o próximo RRN disponível 
        numBytes += 160;  // Cada registro tem um tamanho fixo de 160 bytes

        // Verifica se já foi escrita uma página de disco completa (1600 bytes)
        if ((numBytes % 1600) == 0) {
            contPags++;  // Incrementa o contador de páginas de disco
        }
    }

    // Atualiza o número de páginas de disco no cabeçalho, incluindo a página ocupada pelo cabeçalho e os registros
    header.nroPagDisco = contPags + 1;
    fseek(arquivoBin, 0, SEEK_SET);  // Volta ao início do arquivo para reescrever o cabeçalho atualizado
    header.status = '1';  // Marca o arquivo como consistente (concluído)
    escreverCabecalhoBin(arquivoBin, &header);  // Reescreve o cabeçalho atualizado

    fclose(arquivoCSV);  // Fecha o arquivo CSV
    fclose(arquivoBin);  // Fecha o arquivo binário

    binarioNaTela(nomeArqBin);
}

//--------------------------------------------------------------- FUNCIONALIDADE 2 (SELECT FROM) ---------------------------------------------------------------

void funcionalidade2() {
    Cabecalho header = inicializarCabecalho();  // Inicializa o cabeçalho
    
    char nomeArqBin[30];  // Nome do arquivo binário a ser lido
    scanf("%s", nomeArqBin);
    
    FILE *arquivoBin = fopen(nomeArqBin, "rb");  // Abre o arquivo binário para leitura
    if (arquivoBin == NULL) {
        printf("Falha no processamento do arquivo.");
        return;  // Se o arquivo não for encontrado, a mensagem de erro é exibida e a função termina
    }

    // Lê o cabeçalho do arquivo binário
    lerCabecalhoBin(arquivoBin, &header);

    int flag = 0;  // Flag para verificar se algum registro foi encontrado
    Registro *r = criarRegistro();  // Inicializa o registro que será usado
    r = lerRegistroBin(arquivoBin);  // Lê o primeiro registro do arquivo binário

    // Loop que percorre o arquivo bin até o final
    while (r != NULL) {
        // Verifica se o registro não foi removido
        if (r->removido == '0') {
            flag = 1;  // Define a flag indicando que pelo menos um registro foi encontrado
            // Imprime os campos do registro, ignorando os valores que forem nulos
            if (r->nome[0] != '\0') printf("Nome: %s\n", r->nome);
            if (r->especie[0] != '\0') printf("Especie: %s\n", r->especie);
            if (r->tipo[0] != '\0') printf("Tipo: %s\n", r->tipo);
            if (r->dieta[0] != '\0') printf("Dieta: %s\n", r->dieta);
            if (r->habitat[0] != '\0') printf("Lugar que habitava: %s\n", r->habitat);
            if (r->tamanho != -1) printf("Tamanho: %.1f m\n", r->tamanho);
            if (r->velocidade != -1) printf("Velocidade: %d %cm/h\n", r->velocidade, r->unidadeMedida);
            printf("\n");
        }
        liberarRegistro(r);  // Libera o registro após a impressão
        r = lerRegistroBin(arquivoBin);  // Lê o próximo registro do arquivo
    }

    // Se nenhum registro foi encontrado (ou seja, todos foram removidos), exibe a mensagem de erro
    if (!flag) {
        printf("Registro inexistente.");
    }

    // Exibe o número de páginas de disco ocupadas pelo arquivo
    printf("Numero de paginas de disco: %d\n\n", header.nroPagDisco);

    fclose(arquivoBin);  // Fecha o arquivo binário
}


//--------------------------------------------------------------- FUNCIONALIDADE 3 (SELECT FROM WHERE) ---------------------------------------------------------------

void funcionalidade3() {
    Cabecalho header = inicializarCabecalho();  // Inicializa a estrutura do cabeçalho
    char nomeArqBin[30];  // Nome do arquivo binário que será lido
    int n;  // Número de buscas a serem realizadas

    scanf("%s", nomeArqBin);  // Lê o nome do arquivo binário fornecido pelo usuário
    FILE *arquivoBin = fopen(nomeArqBin, "rb");  // Abre o arquivo binário para leitura
    if (arquivoBin == NULL) {
        printf("Falha no processamento do arquivo.");  // Exibe a mensagem de erro se o arquivo não for encontrado
        return;  // Interrompe a execução
    }

    lerCabecalhoBin(arquivoBin, &header);  // Lê o cabeçalho do arquivo binário 
    scanf("%d", &n);  // Lê o número de tentativas de busca que o usuário deseja realizar

    char campo[30], valor[30];  // Buffers para armazenar o campo e o valor a serem buscados

    // Loop para realizar as 'n' buscas
    for (int i = 0; i < n; i++) {
        scanf("%s", campo);  // Lê o campo pelo qual a busca será realizada (ex: nome, espécie)
        
        // Verifica se o campo contém uma string de tamanho variavel ou não
        if (strcmp(campo, "nome") == 0 || strcmp(campo, "especie") == 0 || strcmp(campo, "habitat") == 0 ||
            strcmp(campo, "dieta") == 0 || strcmp(campo, "tipo") == 0 || strcmp(campo, "alimento") == 0) {
            scan_quote_string(valor);  // Se sim, então utiliza-se a função fornecida para ler o valor
        } else {
            scanf("%s", valor);  // Se não, lê o valor (numérico ou um char)
        }

        // Reposiciona o ponteiro para o início dos registros, logo após o cabeçalho
        fseek(arquivoBin, PAGE_SIZE_T1, SEEK_SET);

        int registrosEncontrados = 0;  // Inicializa o contador de registros encontrados
        Registro *r = criarRegistro(); // Inicializa o registro que será usado
        r = lerRegistroBin(arquivoBin);  // Lê o primeiro registro do arquivo binário

        // Exibe o número da busca
        if (i == 0) {
            printf("Busca %d\n", i + 1);
        } else {
            printf("\nBusca %d\n", i + 1);
        }

        // Loop que percorre o arquivo bin até o final
        while (r != NULL) {
            // Verifica se o registro não foi removido e se o valor do campo está presente entre os registros do arquivo
            if (r->removido == '0' && buscarRegistro(r, campo, valor)) {
                // Exibe os campos do registro encontrado, exceto os nulos
                if (r->nome[0] != '\0') printf("Nome: %s\n", r->nome);
                if (r->especie[0] != '\0') printf("Especie: %s\n", r->especie);
                if (r->tipo[0] != '\0') printf("Tipo: %s\n", r->tipo);
                if (r->dieta[0] != '\0') printf("Dieta: %s\n", r->dieta);
                if (r->habitat[0] != '\0') printf("Lugar que habitava: %s\n", r->habitat);
                if (r->tamanho != -1) printf("Tamanho: %.1f m\n", r->tamanho);
                if (r->velocidade != -1) printf("Velocidade: %d %cm/h\n", r->velocidade, r->unidadeMedida);
                printf("\n");
                registrosEncontrados++; // Incrementa o numero de registros encontrados
            }
            liberarRegistro(r);  // Libera o espaço de memória do registro atual
            r = lerRegistroBin(arquivoBin);  // Lê o próximo registro do arquivo
        }

        // Caso nenhum registro tenha sido encontrado
        if (registrosEncontrados == 0) {
            printf("Registro inexistente.\n\n");
        }

        // Exibe o número de páginas de disco ocupadas pelo arquivo
        printf("Numero de paginas de disco: %d\n", header.nroPagDisco);
    }
    printf("\n");
    fclose(arquivoBin);  // Fecha o arquivo binário
}


//--------------------------------------------------------------- FUNCIONALIDADE 4 (REMOÇÃO LÓGICA) ---------------------------------------------------------------

void funcionalidade4() {
    Cabecalho header = inicializarCabecalho();  // Inicializa o cabeçalho
    char nomeArqBin[30];  // Nome do arquivo binário
    int n;  // Número de registros a serem removidos

    // Lê o nome do arquivo binário fornecido pelo usuário
    scanf("%s", nomeArqBin);
    FILE *arquivoBin = fopen(nomeArqBin, "rb+");  // Abre o arquivo binário para leitura e escrita
    if (arquivoBin == NULL) {
        printf("Falha no processamento do arquivo.");  // Exibe a mensagem de erro se o arquivo não for encontrado
        return;
    }

    // Lê o cabeçalho do arquivo binário
    lerCabecalhoBin(arquivoBin, &header);

    // Atualiza o status do arquivo para inconsistente, pois será modificado
    header.status = '0';

    // Lê o número de registros que serão removidos
    scanf("%d", &n);

    char campo[30], valor[50];  // Buffers para o campo e valor de busca

    // Executa 'n' remoções
    for (int i = 0; i < n; i++) {
        // Lê o campo pelo qual será feita a busca para remoção
        scanf("%s", campo);

        // Verifica se o campo contém uma string de tamanho variavel ou não
        if (strcmp(campo, "nome") == 0 || strcmp(campo, "especie") == 0 || strcmp(campo, "habitat") == 0 ||
            strcmp(campo, "dieta") == 0 || strcmp(campo, "tipo") == 0 || strcmp(campo, "alimento") == 0) {
            scan_quote_string(valor);  // Se sim, então utiliza-se a função fornecida para ler o valor
        } else {
            scanf("%s", valor);  // Se não, lê o valor (numérico ou um char)
        }

        // Reposiciona o ponteiro para o início dos registros, logo após o cabeçalho
        fseek(arquivoBin, PAGE_SIZE_T1, SEEK_SET);

        long int byteOffset;  // Variável para armazenar a posição do registro no arquivo
        Registro *r = criarRegistro(); // Inicializa o registro que será usado
        r = lerRegistroBin(arquivoBin);  // Lê o primeiro registro do arquivo binário
        int RRN = 0;  // Controla o RRN 

        // Loop que percorre o arquivo bin até o final
        while (r != NULL) {
            byteOffset = ftell(arquivoBin);  // Grava o byteOffset do registro

            // Se o registro não foi removido e corresponde ao valor da busca, será removido
            if (r->removido == '0' && buscarRegistro(r, campo, valor)) {
                // Chama a função para remover o registro
                removeRegistro(r, arquivoBin, header.topo, byteOffset, &header.nroRegRem);

                // Atualiza o topo da lista de removidos
                header.topo = ((byteOffset - PAGE_SIZE_T1 - 160) / 160);
            }


            liberarRegistro(r);  // Libera o registro atual
            r = lerRegistroBin(arquivoBin);  // Lê o próximo registro do arquivo
        }
        liberarRegistro(r);  // Libera o último registro lido
    }

    // Marca o status do arquivo como consistente após as remoções
    header.status = '1';

    // Atualiza o número de páginas de disco ocupadas pelo arquivo
    header.nroPagDisco = (ftell(arquivoBin) + PAGE_SIZE_T1 - 1) / PAGE_SIZE_T1;

    // Reescreve o cabeçalho atualizado no arquivo binário
    fseek(arquivoBin, 0, SEEK_SET);
    escreverCabecalhoBin(arquivoBin, &header);

    // Fecha o arquivo e exibe o conteúdo binário atualizado na tela
    fclose(arquivoBin);
    binarioNaTela(nomeArqBin);
}



//--------------------------------------------------------------- FUNCIONALIDADE 5 (INSERT INTO) ---------------------------------------------------------------


void funcionalidade5() {
    Cabecalho header = inicializarCabecalho();  // Inicializa o cabeçalho
    char nomeArqBin[30];  // Variável para armazenar o nome do arquivo binário

    // Lê o nome do arquivo binário
    scanf("%s", nomeArqBin);

    // Lê o nome do arquivo binário fornecido pelo usuário
    FILE *arquivoBin = fopen(nomeArqBin, "rb+");  // Abre o arquivo binário para leitura e escrita
    if (arquivoBin == NULL) {
        printf("Falha no processamento do arquivo.");  // Exibe a mensagem de erro se o arquivo não for encontrado
        return;
    }

    // Lê o cabeçalho do arquivo binário para garantir que os metadados estejam atualizados
    lerCabecalhoBin(arquivoBin, &header);

    // Define o status do cabeçalho como inconsistente, indicando que o arquivo será modificado
    header.status = '0';

    int numInsercoes;  // Variável para armazenar o número de registros a serem inseridos
    scanf("%d", &numInsercoes);  // Lê o número de registros a serem inseridos

    // Loop para realizar a inserção de 'numInsercoes' registros
    for (int i = 0; i < numInsercoes; i++) {
        Registro *r = criarRegistro();  // Cria um novo registro com valores padrão
        char valorCampo[50];  // Buffer para armazenar os valores lidos (strings ou numéricos)

        
        scan_quote_string(valorCampo);  // Lê a string de tamanho variavel 'nome' 
        if (strcmp(valorCampo, "NULO") == 0) {
            strcpy(r->nome, "#");  // Se o valor for "NULO", insere '#'
        } else {
            strcpy(r->nome, valorCampo);  // Caso contrário, copia o valor lido
        }

        
        scan_quote_string(valorCampo);  // Lê a string de tamanho variavel 'dieta' 
        if (strcmp(valorCampo, "NULO") == 0) {
            strcpy(r->dieta, "#");   // Se o valor for "NULO", insere '#'
        } else {
            strcpy(r->dieta, valorCampo);  // Caso contrário, copia o valor lido
        }


        scan_quote_string(valorCampo); // Lê a string de tamanho variavel 'habitat'
        if (strcmp(valorCampo, "NULO") == 0) {
            strcpy(r->habitat, "#");  // Se o valor for "NULO", insere '#'
        } else {
            strcpy(r->habitat, valorCampo);  // Caso contrário, copia o valor lido
        }

        
        scanf("%s", valorCampo);  // Lê o valor do campo 'populacao'
        if (strcmp(valorCampo, "NULO") == 0) {
            r->populacao = -1;  // Se o valor for "NULO", define -1
        } else {
            r->populacao = atoi(valorCampo);  // Caso contrário, converte o valor lido para inteiro
        }

        
        scan_quote_string(valorCampo); // Lê a string de tamanho variavel 'tipo' 
        if (strcmp(valorCampo, "NULO") == 0) {
            strcpy(r->tipo, "#");  // Se o valor for "NULO", insere '#'
        } else {
            strcpy(r->tipo, valorCampo);  // Caso contrário, copia o valor lido
        }

        
        scanf("%s", valorCampo); // Lê o valor do campo 'velocidade'
        if (strcmp(valorCampo, "NULO") == 0) {
            r->velocidade = -1;  // Se o valor for "NULO", define -1
        } else {
            r->velocidade = atoi(valorCampo);  // Converte para inteiro
        }

        
        scanf("%s", valorCampo); // Lê o valor do campo 'unidadeMedida'
        if (strcmp(valorCampo, "NULO") == 0) {
            r->unidadeMedida = '$';  // Atribui '$' para valores nulos
        } else {
            r->unidadeMedida = valorCampo[1];  // Atribui o seungdo caractere da string
        }


        scanf("%s", valorCampo); // Lê o valor do campo 'tamanho'
        if (strcmp(valorCampo, "NULO") == 0) {
            r->tamanho = -1;  // Se o valor for "NULO", define -1
        } else {
            r->tamanho = atof(valorCampo);  // Caso contrário, converte para float
        }

        
        scan_quote_string(valorCampo); // Lê a string de tamanho variavel 'especie'
        if (strcmp(valorCampo, "NULO") == 0) {
            strcpy(r->especie, "#");  // Se o valor for "NULO", insere '#'
        } else {
            strcpy(r->especie, valorCampo);  // Caso contrário, copia o valor lido
        }

        
        scan_quote_string(valorCampo); // Lê a string de tamanho variavel 'alimento'
        if (strcmp(valorCampo, "NULO") == 0) {
            strcpy(r->alimento, "#");  // Se o valor for "NULO", insere '#'
        } else {
            strcpy(r->alimento, valorCampo);  // Caso contrário, copia o valor lido
        }

        // Insere o registro reutilizando o espaço de registros removidos ou ao final do arquivo
        long byteOffSet = inserirRegistro(arquivoBin, r, &header);
        liberarRegistro(r);  // Libera a memória alocada para o registro
    }

    // Move o ponteiro para o final do arquivo para calcular o tamanho
    fseek(arquivoBin, 0, SEEK_END);
    int tamanhoArquivo = ftell(arquivoBin);  // Obtém o tamanho atual do arquivo em bytes

    // Calcula o número de páginas de disco ocupadas
    if (tamanhoArquivo % PAGE_SIZE_T1 == 0) {
        header.nroPagDisco = tamanhoArquivo / PAGE_SIZE_T1; // Se o tamanho do arquivo for um múltiplo de 1600, não se utilizou uma nova página de disco
    } else {
        header.nroPagDisco = (tamanhoArquivo / PAGE_SIZE_T1) + 1; // Caso contrário, utilizou uma página a mais
    }

    // Marca o status do cabeçalho como consistente após a inserção
    header.status = '1';  

    // Reescreve o cabeçalho atualizado no arquivo
    fseek(arquivoBin, 0, SEEK_SET);
    escreverCabecalhoBin(arquivoBin, &header);

    fclose(arquivoBin);  // Fecha o arquivo binário
    binarioNaTela(nomeArqBin); 
}


//--------------------------------------------------------------- FUNCIONALIDADE 6 (COMPACTAÇÃO) ---------------------------------------------------------------

void funcionalidade6() {
    Cabecalho header = inicializarCabecalho();  // Inicializa o cabeçalho
    char nomeArqBin[30];  // Nome do arquivo binário original

    // Lê o nome do arquivo binário
    scanf("%s", nomeArqBin);

    FILE *arquivoBin = fopen(nomeArqBin, "rb+");  // Abre o arquivo binário para leitura e escrita
    if (arquivoBin == NULL) {
        printf("Falha no processamento do arquivo.");  // Exibe a mensagem de erro se o arquivo não for encontrado
        return;
    }

    // Nome do arquivo compactado
    char *nomeArqCompactado = "compactado.bin";
    FILE *arquivoCompactado = fopen(nomeArqCompactado, "wb");  // Cria o arquivo compactado para escrita no modo binário
    if (arquivoCompactado == NULL) {
        printf("Falha ao criar o arquivo compactado.\n");  // // Exibe a mensagem de erro se o arquivo não for encontrado
        fclose(arquivoBin);
        return;
    }

    // Lê o cabeçalho do arquivo original
    lerCabecalhoBin(arquivoBin, &header);

    // Marca o status do arquivo compactado como inconsistente durante a escrita
    header.status = '0';
    escreverCabecalhoBin(arquivoCompactado, &header);

    // Posiciona o ponteiro no início dos registros, logo após o cabeçalho
    fseek(arquivoBin, PAGE_SIZE_T1, SEEK_SET);
    Registro *r;
    long int byteOffset;
    int RRN = 0;  // Inicializa o RRN

    // Itera sobre os registros do arquivo original
    while (1) {
        byteOffset = ftell(arquivoBin);  // Armazena a posição atual do ponteiro
        r = lerRegistroBin(arquivoBin);  // Lê o próximo registro do arquivo original

        if (r == NULL) {
            break;  // Sai do loop quando não há mais registros
        }

        // Apenas registra os que não estão removidos
        if (r->removido == '0') {
            // Escreve o registro no final do arquivo compactado
            escreverRegistro(arquivoCompactado, r);
        }

        RRN++;  // Incrementa o RRN
        liberarRegistro(r);  // Libera a memória alocada para o registro
    }

    header.status = '1'; // Marca o status do arquivo compactado como consistente
    header.topo = -1;  // Não há registros removidos no arquivo compactado
    header.nroRegRem = 0;  // Zera o número de registros removidos
    header.qttCompacta++;  // Incrementa a quantidade de compactações realizadas
    header.proxRRN = RRN;  // Atualiza o próximo RRN disponível

    // Calcula o número de páginas de disco ocupadas pelo arquivo compactado
    fseek(arquivoCompactado, 0, SEEK_END);
    long tamanhoArquivo = ftell(arquivoCompactado);
    header.nroPagDisco = (tamanhoArquivo + PAGE_SIZE_T1 - 1) / PAGE_SIZE_T1;

    // Reescreve o cabeçalho atualizado no arquivo compactado
    fseek(arquivoCompactado, 0, SEEK_SET);
    escreverCabecalhoBin(arquivoCompactado, &header);

    // Fecha os arquivos original e compactado
    fclose(arquivoBin);
    fclose(arquivoCompactado);

    binarioNaTela(nomeArqCompactado);
}


/*

**************************************************************************************************************************************
*                                                                                                                                    *
*                                                             TRABALHO 2                                                             *
*                                                                                                                                    *
**************************************************************************************************************************************

*/


//--------------------------------------------------------------- FUNCIONALIDADE 7 (CREATE INDEX) ---------------------------------------------------------------


void funcionalidade7() {
    // Inicializa os cabeçalhos para o arquivo de dados e o arquivo de índice da árvore B
    Cabecalho headerBin = inicializarCabecalho();  
    CabecalhoIndice headerArvoreB = inicializarCabecalhoIndice();

    // Declara variáveis para armazenar os nomes dos arquivos de dados e de índice
    char nomeArqDados[30];
    char nomeArqIndice[30];

    // Lê os nomes dos arquivos de dados e de índice a partir da entrada do usuário
    scanf("%s", nomeArqDados);
    scanf("%s", nomeArqIndice);

    // Abre o arquivo de dados em modo de leitura binária
    FILE *arqDados = fopen(nomeArqDados, "rb");

    // Verifica se o arquivo de dados foi aberto corretamente
    if (arqDados == NULL) {
        printf("Falha no processamento do arquivo.");
        return; // Encerra a função em caso de erro
    }

    // Lê o cabeçalho do arquivo de dados e armazena as informações em headerBin
    lerCabecalhoBin(arqDados, &headerBin);

    // Verifica o status do cabeçalho do arquivo de dados
    if (headerBin.status == 0) {
        printf("Falha no processamento do arquivo.");
        fclose(arqDados);
        return; // Encerra a função em caso de erro no status do arquivo
    }

    // Abre o arquivo de índice da árvore B em modo de escrita binária
    FILE *arqIndice = fopen(nomeArqIndice, "wb+");

    // Escreve o cabeçalho inicial da árvore B no arquivo de índice
    escreverCabecalhoIndice(arqIndice, &headerArvoreB);

    // Declara variáveis para as operações de inserção e promoção
    long chavePromovida, referenciaPromovida; 
    int filhoDireitaPromovida;

    // Cria e lê o primeiro registro do arquivo de dados
    Registro *r = criarRegistro();
    r = lerRegistroBin(arqDados);
    long byteOffset = ftell(arqDados) - 160; // Calcula a posição inicial no arquivo de dados

    // Processa cada registro do arquivo de dados enquanto ele não for nulo
    while (r != NULL) {
        long byteOffset = ftell(arqDados) - 160; // Calcula o byte offset para o registro atual
        
        // Verifica se o registro não foi removido
        if (r->removido == '0') {
            // Converte o campo nome do registro para uma chave numérica
            long chave = converteNome(r->nome);

            // Se a árvore B ainda não possui uma raiz, inicializa a primeira página como raiz
            if (headerArvoreB.noRaiz == -1) {
                Pagina *primeiraPag = criaPagina();
                primeiraPag->folha = '1'; // Marca a página como folha
                primeiraPag->chaves[0] = chave; // Insere a chave na primeira posição
                primeiraPag->referencias[0] = byteOffset; // Armazena o byteOffset do registro
                primeiraPag->nroChavesIndexadas++; // Define o número de chaves indexadas como 1
                primeiraPag->RRNdoNo = headerArvoreB.RRNproxNo++; // Atribui o próximo RRN
                
                // Posiciona o ponteiro no arquivo de índice e grava a página inicial
                fseek(arqIndice, OFFSET_CABECALHO_ARVB + (primeiraPag->RRNdoNo * TAM_PAGINA), SEEK_SET);
                escrevePaginaBin(arqIndice, primeiraPag);
                headerArvoreB.noRaiz = primeiraPag->RRNdoNo; // Define o RRN da raiz da árvore B
                free(primeiraPag); // Libera a memória da página
            } else {
                // Verifica se a chave não está presente na árvore
                if (busca(arqIndice, headerArvoreB.noRaiz, chave) == NAO_ENCONTRADO) {
                    // Realiza a inserção da chave na árvore
                    int resultadoInsercao = inserir(arqIndice, headerArvoreB.noRaiz, chave, byteOffset,
                                &chavePromovida, &referenciaPromovida, &filhoDireitaPromovida, &headerArvoreB.RRNproxNo);

                    // Se a inserção resulta em uma promoção (split)
                    if (resultadoInsercao == PROMOCAO) {
                        // Cria uma nova raiz para a árvore B
                        Pagina *novaRaiz = criaPagina();
                        novaRaiz->chaves[0] = chavePromovida; // Atribui a chave promovida
                        novaRaiz->referencias[0] = referenciaPromovida; // Atribui a referência promovida
                        novaRaiz->ponteiros[0] = headerArvoreB.noRaiz; // Atualiza o ponteiro para a raiz anterior
                        novaRaiz->ponteiros[1] = filhoDireitaPromovida; // Atualiza o ponteiro para o nó da direita
                        novaRaiz->nroChavesIndexadas = 1; // Define o número de chaves na nova raiz
                        novaRaiz->folha = '0'; // Marca a nova raiz como nó não-folha
                        novaRaiz->RRNdoNo = headerArvoreB.RRNproxNo++; // Atribui o RRN à nova raiz

                        // Posiciona o ponteiro no arquivo de índice e grava a nova raiz
                        fseek(arqIndice, OFFSET_CABECALHO_ARVB + novaRaiz->RRNdoNo * TAM_PAGINA, SEEK_SET);
                        escrevePaginaBin(arqIndice, novaRaiz);
                        headerArvoreB.noRaiz = novaRaiz->RRNdoNo; // Atualiza o RRN da raiz no cabeçalho
                        free(novaRaiz); // Libera a memória da nova raiz
                    }
                }
            }
        }

        // Libera o registro atual e lê o próximo registro
        liberarRegistro(r);
        r = lerRegistroBin(arqDados);
    }

    // Atualiza o status do cabeçalho da árvore B e grava no início do arquivo de índice
    headerArvoreB.status = '1';
    fseek(arqIndice, 0, SEEK_SET);
    escreverCabecalhoIndice(arqIndice, &headerArvoreB);

    // Fecha os arquivos de dados e de índice
    fclose(arqDados);
    fclose(arqIndice);
    // Exibe o conteúdo binário do arquivo de índice na tela
    binarioNaTela(nomeArqIndice);
}



//--------------------------------------------------------------- FUNCIONALIDADE 8 (RECUPERAÇÃO DOS DADOS) ---------------------------------------------------------------

void funcionalidade8() {
    // Declara variáveis para armazenar os nomes dos arquivos e valores de entrada.
    char nomeArqDados[30], nomeArqIndice[30], nomeCampo[30], valorCampoStr[100];
    long valorCampoLong;

    // Leitura dos parâmetros do usuário.
    scanf("%s", nomeArqDados);
    scanf("%s", nomeArqIndice);
    scanf("%s", nomeCampo);
    scan_quote_string(valorCampoStr);

    // Converte o valor do campo 'nome' para um long para buscar na árvore B.
    valorCampoLong = converteNome(valorCampoStr);

    // Abre o arquivo de índice em modo de leitura binária.
    FILE *arqIndice = fopen(nomeArqIndice, "rb");
    if (arqIndice == NULL) {
        printf("Falha no processamento do arquivo.");
        return;
    }

    // Lê o cabeçalho do índice e verifica o status para garantir que está consistente.
    CabecalhoIndice cabecalhoArvore = inicializarCabecalhoIndice();
    lerCabecalhoIndice(arqIndice, &cabecalhoArvore);

    if (cabecalhoArvore.status == '0') {
        printf("Falha no processamento do arquivo.");
        fclose(arqIndice);
        return;
    } else if (cabecalhoArvore.noRaiz == -1) {
        // Caso a árvore não tenha raiz, significa que está vazia.
        printf("Registro inexistente.");
        fclose(arqIndice);
        return;
    }

    // Chama a função `busca` para encontrar o valor da chave no índice.
    long byteOffset = busca(arqIndice, cabecalhoArvore.noRaiz, valorCampoLong);
    if (byteOffset == NAO_ENCONTRADO) {
        printf("Registro inexistente.");
        fclose(arqIndice);
        return;
    }

    // Abre o arquivo de dados em modo de leitura binária.
    FILE *arqDados = fopen(nomeArqDados, "rb");
    if (arqDados == NULL) {
        printf("Falha no processamento do arquivo.");
        fclose(arqIndice);
        return;
    }

    // Lê o cabeçalho do arquivo de dados e verifica o status para garantir consistência.
    Cabecalho cabecalhoBin = inicializarCabecalho();
    lerCabecalhoBin(arqDados, &cabecalhoBin);
    
    if (cabecalhoBin.status == '0') {
        printf("Falha no processamento do arquivo.");
        fclose(arqIndice);
        fclose(arqDados);
        return;
    }

    // Posiciona no byte offset obtido e lê o registro correspondente no arquivo de dados.
    fseek(arqDados, byteOffset, SEEK_SET);
    Registro *r = lerRegistroBin(arqDados);
    
    if (r->removido == '0') {
        // Exibe os campos do registro encontrado, verificando o preenchimento de cada campo.
        if (r->nome[0] != '\0') printf("Nome: %s\n", r->nome);
        if (r->especie[0] != '\0') printf("Especie: %s\n", r->especie);
        if (r->tipo[0] != '\0') printf("Tipo: %s\n", r->tipo);
        if (r->dieta[0] != '\0') printf("Dieta: %s\n", r->dieta);
        if (r->habitat[0] != '\0') printf("Lugar que habitava: %s\n", r->habitat);
        if (r->tamanho != -1) printf("Tamanho: %.1f m\n", r->tamanho);
        if (r->velocidade != -1) printf("Velocidade: %d %cm/h\n", r->velocidade, r->unidadeMedida);
        printf("\n");
    } else {
        // Exibe mensagem se o registro está logicamente removido.
        printf("Registro inexistente.");
    }

    // Fecha os arquivos e libera a memória alocada.
    liberarRegistro(r);
    fclose(arqIndice);
    fclose(arqDados);
}

//--------------------------------------------------------------- FUNCIONALIDADE 9 (INSERT EXTENDIDA) ---------------------------------------------------------------


void funcionalidade9() {
    char nomeArqDados[30];
    char nomeArqIndice[30];
    
    // Lê o nome dos arquivos de dados e de índice a partir da entrada do usuário
    scanf("%s", nomeArqDados);
    scanf("%s", nomeArqIndice);

    // Abre o arquivo de dados em modo leitura e escrita binária
    FILE *arqDados = fopen(nomeArqDados, "rb+");
    if (arqDados == NULL) {
        printf("Falha no processamento do arquivo.");
        return; // Retorna em caso de erro ao abrir o arquivo de dados
    }

    // Abre o arquivo de índice em modo leitura e escrita binária
    FILE *arqIndice = fopen(nomeArqIndice, "rb+");
    if (arqIndice == NULL) {
        printf("Falha no processamento do arquivo.");
        fclose(arqDados); // Fecha o arquivo de dados antes de retornar
        return;
    }

    // Inicializa e lê os cabeçalhos do arquivo de dados e do arquivo de índice da árvore B
    Cabecalho cabecalhoArqBin = inicializarCabecalho();
    CabecalhoIndice cabecalhoArvB = inicializarCabecalhoIndice();

    // Carrega os cabeçalhos a partir dos arquivos
    lerCabecalhoBin(arqDados, &cabecalhoArqBin);
    lerCabecalhoIndice(arqIndice, &cabecalhoArvB);

    // Marca ambos os arquivos como inconsistentes para indicar operação em andamento
    cabecalhoArqBin.status = '0';
    cabecalhoArvB.status = '0';

    // Lê o número de registros que serão inseridos
    int numInsercoes;
    scanf("%d", &numInsercoes);

    for (int i = 0; i < numInsercoes; i++) {
        
        // Cria um novo registro com valores padrão
        Registro *r = criarRegistro();
        char valorCampo[50];  // Buffer temporário para armazenar valores de campos

        // Lê o valor do campo 'nome' (campo de string de tamanho variável)
        scan_quote_string(valorCampo);
        if (strcmp(valorCampo, "NULO") == 0) {
            strcpy(r->nome, "#");  // Se o valor for "NULO", insere '#'
        } else {
            strcpy(r->nome, valorCampo);  // Copia o valor lido para o campo 'nome' do registro
        }

        // Lê o valor do campo 'dieta' (campo de string de tamanho variável)
        scan_quote_string(valorCampo);
        if (strcmp(valorCampo, "NULO") == 0) {
            strcpy(r->dieta, "#");  // Insere '#' para valor "NULO"
        } else {
            strcpy(r->dieta, valorCampo);  // Copia o valor lido
        }

        // Lê o valor do campo 'habitat'
        scan_quote_string(valorCampo);
        if (strcmp(valorCampo, "NULO") == 0) {
            strcpy(r->habitat, "#");
        } else {
            strcpy(r->habitat, valorCampo);
        }

        // Lê o valor do campo 'populacao' (campo numérico)
        scanf("%s", valorCampo);
        if (strcmp(valorCampo, "NULO") == 0) {
            r->populacao = -1;  // Define -1 para valor nulo
        } else {
            r->populacao = atoi(valorCampo);  // Converte o valor lido para inteiro
        }

        // Lê o valor do campo 'tipo' (campo de string de tamanho variável)
        scan_quote_string(valorCampo);
        if (strcmp(valorCampo, "NULO") == 0) {
            strcpy(r->tipo, "#");
        } else {
            strcpy(r->tipo, valorCampo);
        }

        // Lê o valor do campo 'velocidade' (campo numérico)
        scanf("%s", valorCampo);
        if (strcmp(valorCampo, "NULO") == 0) {
            r->velocidade = -1;
        } else {
            r->velocidade = atoi(valorCampo);
        }

        // Lê o valor do campo 'unidadeMedida' (caractere de unidade de medida)
        scanf("%s", valorCampo);
        if (strcmp(valorCampo, "NULO") == 0) {
            r->unidadeMedida = '$';  // Usa '$' para valor nulo
        } else {
            r->unidadeMedida = valorCampo[1];  // Extrai o segundo caractere
        }

        // Lê o valor do campo 'tamanho' (campo numérico de ponto flutuante)
        scanf("%s", valorCampo);
        if (strcmp(valorCampo, "NULO") == 0) {
            r->tamanho = -1;  // Define -1 para valor nulo
        } else {
            r->tamanho = atof(valorCampo);  // Converte o valor lido para float
        }

        // Lê o valor do campo 'especie' (campo de string de tamanho variável)
        scan_quote_string(valorCampo);
        if (strcmp(valorCampo, "NULO") == 0) {
            strcpy(r->especie, "#");
        } else {
            strcpy(r->especie, valorCampo);
        }

        // Lê o valor do campo 'alimento' (campo de string de tamanho variável)
        scan_quote_string(valorCampo);
        if (strcmp(valorCampo, "NULO") == 0) {
            strcpy(r->alimento, "#");
        } else {
            strcpy(r->alimento, valorCampo);
        }

        // Converte o nome para uma chave numérica
        long chave = converteNome(r->nome);
        // Insere o registro no arquivo de dados e recebe o byte offset
        long byteOffset = inserirRegistro(arqDados, r, &cabecalhoArqBin); 
         // Ajusta o byte offset, se necessário

        // Verifica se houve erro ao inserir o registro
        if (byteOffset == ERRO) {
            printf("Falha no processamento do arquivo.");
            liberarRegistro(r);
            fclose(arqDados);
            fclose(arqIndice);
            return;
        }

        // Verifica se a árvore B ainda não possui uma raiz
        if (cabecalhoArvB.noRaiz == -1) {
            // Inicializa a primeira página raiz da árvore B
            Pagina *primeiraPag = criaPagina();
            primeiraPag->folha = '1';
            primeiraPag->chaves[0] = chave;
            primeiraPag->referencias[0] = byteOffset;
            primeiraPag->nroChavesIndexadas = 1;
            primeiraPag->RRNdoNo = cabecalhoArvB.RRNproxNo++;
            escrevePaginaBin(arqIndice, primeiraPag);

            cabecalhoArvB.noRaiz = primeiraPag->RRNdoNo;
            free(primeiraPag);
        } else {
            // Declara variáveis para promover chaves e referências
            long chavePromovida, referenciaPromovida;
            int filhoDireitaPromovida;
            
            // Realiza uma busca no índice para verificar se a chave já existe no arquivo de índice
            // Se a chave não estiver presente, inicia o processo de inserção na árvore-B
            if (busca(arqIndice, cabecalhoArvB.noRaiz, chave) == NAO_ENCONTRADO) {
                
                // Chama a função de inserção na árvore-B, passando a raiz atual e a chave a ser inserida.
                // Se a função retornar 'PROMOCAO', indica que o nó ficou cheio e houve uma promoção de chave.
                // Esta promoção exige que uma nova raiz seja criada para manter o balanceamento da árvore.
                if (inserir(arqIndice, cabecalhoArvB.noRaiz, chave, byteOffset,
                            &chavePromovida, &referenciaPromovida,
                            &filhoDireitaPromovida, &cabecalhoArvB.RRNproxNo) == PROMOCAO) {
                    
                    // Cria uma nova página que será a nova raiz da árvore-B.
                    // A nova raiz conterá a chave promovida no meio, e seus ponteiros apontarão para
                    // os nós resultantes do split (nó atual e o novo nó à direita).
                    Pagina *novaRaiz = criaPagina();
                    
                    // Define a chave promovida como a primeira chave da nova raiz.
                    novaRaiz->chaves[0] = chavePromovida;
                    
                    // Armazena a referência correspondente à chave promovida.
                    novaRaiz->referencias[0] = referenciaPromovida;
                    
                    // Atualiza os ponteiros da nova raiz para apontar para os filhos gerados no split.
                    // O ponteiro à esquerda aponta para a raiz antiga, e o ponteiro à direita aponta para o novo nó.
                    novaRaiz->ponteiros[0] = cabecalhoArvB.noRaiz;     // Raiz anterior torna-se o filho à esquerda
                    novaRaiz->ponteiros[1] = filhoDireitaPromovida;    // Novo nó resultante do split à direita
                    
                    // A nova raiz agora possui uma chave e não é mais uma folha.
                    novaRaiz->nroChavesIndexadas = 1;
                    novaRaiz->folha = '0'; // Marca como nó interno
                    
                    // Define o RRN da nova raiz e incrementa o próximo RRN disponível no cabeçalho.
                    novaRaiz->RRNdoNo = cabecalhoArvB.RRNproxNo++;
                    
                    // Escreve a nova raiz no arquivo de índice.
                    escrevePaginaBin(arqIndice, novaRaiz);
                    
                    // Atualiza o cabeçalho da árvore-B para apontar para a nova raiz.
                    cabecalhoArvB.noRaiz = novaRaiz->RRNdoNo;
                    
                    // Libera a memória alocada para a nova raiz.
                    free(novaRaiz);
                }
            }
        }

        liberarRegistro(r); // Libera a memória do registro atual
    }

    // Marca o fim da operação, atualizando o status dos cabeçalhos para 'consistente'
    cabecalhoArqBin.status = '1';
    cabecalhoArvB.status = '1';
    fseek(arqDados, 0, SEEK_SET);
    escreverCabecalhoBin(arqDados, &cabecalhoArqBin);
    fseek(arqIndice, 0, SEEK_SET);
    escreverCabecalhoIndice(arqIndice, &cabecalhoArvB);

    // Fecha os arquivos de dados e de índice
    fclose(arqDados);
    fclose(arqIndice);
    // Exibe o conteúdo do arquivo de índice na tela
    binarioNaTela(nomeArqIndice);
}

