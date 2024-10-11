#include <ctype.h>
#include <string.h>
#include <math.h>
#include "funcoesFornecidas.h"
#include "cabecalho.h"
#include "registro.h"
#include "funcoesArvoreB.h"

#define DELIMITER '#'
#define TRASH '$'
#define PAGE_SIZE 1600

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
        fseek(arquivoBin, PAGE_SIZE, SEEK_SET);

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
        fseek(arquivoBin, PAGE_SIZE, SEEK_SET);

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
                header.topo = ((byteOffset - PAGE_SIZE - 160) / 160);
            }


            liberarRegistro(r);  // Libera o registro atual
            r = lerRegistroBin(arquivoBin);  // Lê o próximo registro do arquivo
        }
        liberarRegistro(r);  // Libera o último registro lido
    }

    // Marca o status do arquivo como consistente após as remoções
    header.status = '1';

    // Atualiza o número de páginas de disco ocupadas pelo arquivo
    header.nroPagDisco = (ftell(arquivoBin) + PAGE_SIZE - 1) / PAGE_SIZE;

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
        inserirRegistro(arquivoBin, r, &header);
        liberarRegistro(r);  // Libera a memória alocada para o registro
    }

    // Move o ponteiro para o final do arquivo para calcular o tamanho
    fseek(arquivoBin, 0, SEEK_END);
    int tamanhoArquivo = ftell(arquivoBin);  // Obtém o tamanho atual do arquivo em bytes

    // Calcula o número de páginas de disco ocupadas
    if (tamanhoArquivo % PAGE_SIZE == 0) {
        header.nroPagDisco = tamanhoArquivo / PAGE_SIZE; // Se o tamanho do arquivo for um múltiplo de 1600, não se utilizou uma nova página de disco
    } else {
        header.nroPagDisco = (tamanhoArquivo / PAGE_SIZE) + 1; // Caso contrário, utilizou uma página a mais
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
    fseek(arquivoBin, PAGE_SIZE, SEEK_SET);
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
    header.nroPagDisco = (tamanhoArquivo + PAGE_SIZE - 1) / PAGE_SIZE;

    // Reescreve o cabeçalho atualizado no arquivo compactado
    fseek(arquivoCompactado, 0, SEEK_SET);
    escreverCabecalhoBin(arquivoCompactado, &header);

    // Fecha os arquivos original e compactado
    fclose(arquivoBin);
    fclose(arquivoCompactado);

    binarioNaTela(nomeArqCompactado);
}


//--------------------------------------------------------------- FUNCIONALIDADE 7 (CREATE INDEX) ---------------------------------------------------------------


void funcionalidade7(){

    Cabecalho headerBin = inicializarCabecalho();  // Inicializa o cabeçalho
    CabecalhoArvoreB headerArvoreB = inicializarCabecalhoArvoreB();

    char nomeArqDados[30];
    char nomeArqIndice[30];

    scanf("%s", nomeArqDados);
    scanf("%s", nomeArqIndice);

    FILE *arqDados = fopen(nomeArqDados, "rb");

    if (arqDados == NULL) {
        printf("Falha no processamento do arquivo.");
        return; // erro
    }

    lerCabecalhoBin(arqDados, &headerBin);

    if (headerBin.status == 0) {
        printf("Falha no processamento do arquivo.");
        fclose(arqDados);
        return; // erro
    }

    FILE *arqIndice = fopen(nomeArqIndice, "wb+");

    // escreve o cabecalho para deixar seu espaco reservado
    escreverCabecalhoArvoreB(arqIndice, &headerArvoreB);

    long chavePromovida, referenciaPromovida; 
    int filhoDireitaPromovida;
    long byteOffset = ftell(arqDados);

    Registro *r = lerRegistroBin(arqDados);
    // Leitura do arquivo de dados e inserção na árvore-B
    while (r != NULL) {

        if (r->removido == '0'){
            // Converte o nome para o tipo `long` para ser a chave
            long chave = converteNome(r->nome);

            // Se o registro não está logicamente removido, insere na árvore-B
            if (headerArvoreB.noRaiz == -1) {
                // Inicializa a primeira página (raiz) se a árvore estiver vazia
                RegistroArvoreB *primeiraPag = criaPagina();
                primeiraPag->folha = '1';
                primeiraPag->chaves[0] = chave;
                primeiraPag->referencias[0] = byteOffset;
                primeiraPag->nroChavesIndexadas = 1;
                primeiraPag->RRNdoNo = headerArvoreB.RRNproxNo++;

                // Posiciona o ponteiro do arquivo antes de escrever a raiz
                fseek(arqIndice, OFFSET_CABECALHO_ARVB + (primeiraPag->RRNdoNo * TAM_PAGINA), SEEK_SET);
                escrevePaginaBin(arqIndice, primeiraPag);

                headerArvoreB.noRaiz = primeiraPag->RRNdoNo;

                free(primeiraPag);
            } else {
                // Se a chave não existe, insere na árvore-B
                if (busca(arqIndice, headerArvoreB.noRaiz, chave) == NAO_ENCONTRADO) {
                    int resultadoInsercao = inserir(arqIndice, headerArvoreB.noRaiz, chave, referenciaPromovida,
                                &chavePromovida, &referenciaPromovida, &filhoDireitaPromovida, &headerArvoreB.RRNproxNo);


                    if (resultadoInsercao == PROMOCAO) {
                        // Create a new root
                        RegistroArvoreB *novaRaiz = criaPagina();
                        novaRaiz->chaves[0] = chavePromovida;
                        novaRaiz->referencias[0] = referenciaPromovida;
                        novaRaiz->ponteiros[0] = headerArvoreB.noRaiz;
                        novaRaiz->ponteiros[1] = filhoDireitaPromovida;
                        novaRaiz->nroChavesIndexadas = 1;
                        novaRaiz->folha = '0';
                        novaRaiz->RRNdoNo = headerArvoreB.RRNproxNo++;

                        // Write new root to file
                        fseek(arqIndice, OFFSET_CABECALHO_ARVB + novaRaiz->RRNdoNo * TAM_PAGINA, SEEK_SET);
                        escrevePaginaBin(arqIndice, novaRaiz);

                        headerArvoreB.noRaiz = novaRaiz->RRNdoNo;
                        free(novaRaiz);
                    }


                }
            }
        }

        // Atualiza o byte offset para o próximo registro
        byteOffset = ftell(arqDados);
        liberarRegistro(r);
        r = lerRegistroBin(arqDados);
    }

    liberarRegistro(r);
    // Marca o arquivo de índice como consistente e reescreve o cabeçalho
    headerArvoreB.status = '1';
    fseek(arqIndice, 0, SEEK_SET);
    escreverCabecalhoArvoreB(arqIndice, &headerArvoreB);

    // Fecha os arquivos
    fclose(arqDados);
    fclose(arqIndice);

    // Exibe o arquivo binário de índice usando a função binarioNaTela
    binarioNaTela(nomeArqIndice);
}


//--------------------------------------------------------------- FUNCIONALIDADE 8 (RECUPERAÇÃO DOS DADOS) ---------------------------------------------------------------

void funcionalidade8(){

}

//--------------------------------------------------------------- FUNCIONALIDADE 9 (INSERT EXTENDIDA) ---------------------------------------------------------------

void funcionalidade9(){

}