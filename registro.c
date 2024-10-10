#include <ctype.h>
#include <string.h>
#include "funcoesFornecidas.h"
#include "registro.h"
#include "funcoesUteis.h"
#include "cabecalho.h"

#define DELIMITER '#'
#define TRASH '$'
#define PAGE_SIZE 1600

// Função para liberar a memória de um registro
void liberarRegistro(Registro *reg) {
    if (reg == NULL) return;

    // Libera cada string se não for NULL
    if (reg->nome) free(reg->nome);
    if (reg->especie) free(reg->especie);
    if (reg->habitat) free(reg->habitat);
    if (reg->tipo) free(reg->tipo);
    if (reg->dieta) free(reg->dieta);
    if (reg->alimento) free(reg->alimento);

    // Libera a estrutura principal
    free(reg);
}

// Função para criar um novo registro com valores padrão
// A função aloca memória para o registro e inicializa seus campos com valores padrão
Registro *criarRegistro() {
    // Aloca memória para um novo registro
    Registro *novoRegistro = (Registro *) malloc(sizeof(Registro));
    
    // Verifica se a alocação de memória foi bem-sucedida
    if (novoRegistro == NULL) return NULL;

    // Inicializa os valores padrão dos campos do registro
    novoRegistro->removido = '0';  // Indica que o registro não foi removido
    novoRegistro->encadeamento = -1;  // Sem encadeamento (-1), pois o registro é novo
    novoRegistro->populacao = -1;  // Valor padrão de inicialização para 'populacao' 
    novoRegistro->tamanho = -1;  // Valor padrão de inicialização para 'tamanho' 
    novoRegistro->unidadeMedida = '$';  // Valor nulo para unidade de medida
    novoRegistro->velocidade = -1;  // Valor padrão de inicialização para 'velocidade'

    // Aloca memória para os campos variáveis (strings)
    // Foi definido um limite de 50 caracteres para cada campo de string
    novoRegistro->nome = (char *) malloc(sizeof(char) * 50);
    novoRegistro->especie = (char *) malloc(sizeof(char) * 50);
    novoRegistro->habitat = (char *) malloc(sizeof(char) * 50);
    novoRegistro->tipo = (char *) malloc(sizeof(char) * 50);
    novoRegistro->dieta = (char *) malloc(sizeof(char) * 50);
    novoRegistro->alimento = (char *) malloc(sizeof(char) * 50);

    // Verifica se as alocações de memória para os campos variáveis foram bem-sucedidas
    // Se algum dos campos falhar, libera a memória alocada para o registro inteiro e retorna NULL
    if (!novoRegistro->nome || !novoRegistro->especie || !novoRegistro->habitat ||
        !novoRegistro->tipo || !novoRegistro->dieta || !novoRegistro->alimento) {
        liberarRegistro(novoRegistro);  // Libera toda a memória alocada em caso de falha
        return NULL;
    }

    // Retorna o ponteiro para o novo registro alocado e inicializado
    return novoRegistro;
}


// Função para ler uma linha do arquivo CSV e criar um registro a partir dela
// Retorna um ponteiro para o registro criado ou NULL em caso de falha ou fim de arquivo
Registro *lerRegistroCSV(FILE *arquivo) {
    
    // Verifica se o ponteiro para o arquivo é nulo ou se o arquivo chegou ao final
    if (arquivo == NULL || feof(arquivo)) return NULL;

    // Cria um novo registro com valores padrão
    Registro *r = criarRegistro();

    char linhaCSV[256];  // Buffer para armazenar a linha lida do arquivo CSV
    // Lê a linha do arquivo até o caractere de nova linha '\n'
    fscanf(arquivo, "%[^\n]\n", linhaCSV);

    int i = 0, k = 0;  // Variáveis para iterar pela linha e pelos campos
    char stringNumerica[20];  // Buffer para armazenar strings numéricas temporariamente

    // Leitura do campo "nome" (não pode ser nulo, garantido pelo CSV)
    // Percorre a string até encontrar a vírgula, que indica o final do campo
    for (k = 0; linhaCSV[i] != ','; i++, k++) r->nome[k] = linhaCSV[i];
    r->nome[k] = '\0';  // Finaliza a string com o demarcador '\0' (temporário)
    i++;  // Avança para o próximo campo

    // Leitura do campo "dieta" (pode ser nulo)
    if (linhaCSV[i] == ',') {
        r->dieta[0] = '#'; // Se o campo for nulo, marca com '#'
        r->dieta[1] = '\0'; // Finaliza a string com o demarcador '\0' (temporário)
        i++;  // Avança para o próximo campo
    } else {
        // Caso não seja nulo, lê até a próxima vírgula
        for (k = 0; linhaCSV[i] != ','; i++, k++) r->dieta[k] = linhaCSV[i];
        r->dieta[k] = '\0'; // Finaliza a string com o demarcador '\0' (temporário)
        i++;  // Avança para o próximo campo
    }

    // Leitura do campo "habitat" (pode ser nulo)
    if (linhaCSV[i] == ',') {
        r->habitat[0] = '#';  // Marca com '#' caso seja nulo
        r->habitat[1] = '\0'; // Finaliza a string com o demarcador '\0' (temporário)
        i++;  // Avança para o próximo campo
    } else {
        // Caso não seja nulo, lê até a próxima vírgula
        for (k = 0; linhaCSV[i] != ','; i++, k++) r->habitat[k] = linhaCSV[i];
        r->habitat[k] = '\0'; // Finaliza a string com o demarcador '\0' (temporário)
        i++;  // Avança para o próximo campo
    }

    // Leitura do campo "populacao" (pode ser nulo)
    if (linhaCSV[i] == ',') {
        r->populacao = -1;  // Representa valor nulo com -1
        i++;  // Avança para o próximo campo
    } else {
        // Caso não seja nulo, lê o valor como string e converte para inteiro
        for (k = 0; linhaCSV[i] != ','; i++, k++) stringNumerica[k] = linhaCSV[i];
        stringNumerica[k] = '\0'; // Finaliza a string com o demarcador '\0' (temporário)
        r->populacao = atoi(stringNumerica);  // Converte a string para inteiro
        i++;  // Avança para o próximo campo
    }

    // Leitura do campo "tipo" (pode ser nulo)
    if (linhaCSV[i] == ',') {
        r->tipo[0] = '#';  // Marca com '#' caso seja nulo
        r->tipo[1] = '\0'; // Finaliza a string com o demarcador '\0' (temporário)
        i++;  // Avança para o próximo campo
    } else {
        // Caso não seja nulo, lê até a próxima vírgula
        for (k = 0; linhaCSV[i] != ','; i++, k++) r->tipo[k] = linhaCSV[i];
        r->tipo[k] = '\0'; // Finaliza a string com o demarcador '\0' (temporário)
        i++;  // Avança para o próximo campo
    }

    // Leitura do campo "velocidade" (pode ser nulo)
    if (linhaCSV[i] == ',') {
        r->velocidade = -1;  // Representa valor nulo com -1
        i++;  // Avança para o próximo campo
    } else {
        // Caso não seja nulo, lê o valor como string e converte para inteiro
        for (k = 0; linhaCSV[i] != ','; i++, k++) stringNumerica[k] = linhaCSV[i];
        stringNumerica[k] = '\0';  // Finaliza a string com o demarcador '\0' (temporário)
        r->velocidade = atoi(stringNumerica);  // Converte a string para inteiro
        i++;  // Avança para o próximo campo
    }

    // Leitura do campo "unidadeMedida" (pode ser nulo)
    if (linhaCSV[i] == ',') {
        r->unidadeMedida = '$';  // Representa valor nulo com '$'
        i++;  // Avança para o próximo campo
    } else {
        r->unidadeMedida = linhaCSV[i];  // Lê o caractere da unidade de medida
        i += 2;  // Avança a vírgula e o caractere
    }

    // Leitura do campo "tamanho" (pode ser nulo)
    if (linhaCSV[i] == ',') {
        r->tamanho = -1;  // Representa valor nulo com -1
        i++;  // Avança para o próximo campo
    } else {
        // Caso não seja nulo, lê o valor como string e converte para float
        for (k = 0; linhaCSV[i] != ','; i++, k++) stringNumerica[k] = linhaCSV[i];
        stringNumerica[k] = '\0'; // Finaliza a string com o demarcador '\0' (temporário)
        r->tamanho = atof(stringNumerica);  // Converte a string para float
        i++;  // Avança para o próximo campo
    }

    // Leitura do campo "especie" (não pode ser nulo, garantido pelo CSV)
    for (k = 0; linhaCSV[i] != ','; i++, k++) r->especie[k] = linhaCSV[i];
    r->especie[k] = '\0';  // Finaliza a string "especie"
    i++;  // Avança para o próximo campo

    // Leitura do campo "alimento" (pode ser nulo)
    if (linhaCSV[i] == '\n' || linhaCSV[i] == '\r' || linhaCSV[i] == '\0') {
        r->alimento[0] = '#';  // Representa campo nulo com '#'
        r->alimento[1] = '\0'; // Finaliza a string com o demarcador '\0' (temporário)
    } else {
        // Caso não seja nulo, lê até o final da linha ou quebra de linha
        for (k = 0; linhaCSV[i] != '\n' && linhaCSV[i] != '\r' && linhaCSV[i] != '\0'; i++, k++) r->alimento[k] = linhaCSV[i];
        r->alimento[k] = '\0';  // Finaliza a string com o demarcador '\0' (temporário)
    }

    // Retorna o registro criado com os campos preenchidos
    return r;
}

// Função para escrever um registro no arquivo binário
// A função grava os campos do registro no arquivo e preenche o restante com um caractere de lixo ('$')
void escreverRegistro(FILE *arquivo, Registro *reg) {
    // Verifica se o ponteiro do arquivo ou o registro é nulo, evitando erros
    if(arquivo == NULL || reg == NULL) return;

    char divider = DELIMITER;  // Caractere delimitador para campos variáveis (usado após strings)
    char trash = TRASH;  // Caractere para preencher o espaço não utilizado
    int tamanhoUsado = 0;  // Variável para rastrear o espaço utilizado em bytes

    // Escreve o campo 'removido' 
    fwrite(&reg->removido, sizeof(char), 1, arquivo);
    tamanhoUsado += sizeof(char);  // Atualiza o tamanho usado após cada escrita

    // Escreve o campo 'encadeamento'
    fwrite(&reg->encadeamento, sizeof(int), 1, arquivo);
    tamanhoUsado += sizeof(int);

    // Escreve o campo 'populacao', tratando valor nulo como -1
    if (reg->populacao == -1) {
        int valorNulo = -1;
        fwrite(&valorNulo, sizeof(int), 1, arquivo);  // Escreve valor nulo
    } else {
        fwrite(&reg->populacao, sizeof(int), 1, arquivo);  // Escreve o valor da população
    }
    tamanhoUsado += sizeof(int);

    // Escreve o campo 'tamanho', tratando valor nulo como -1
    if (reg->tamanho == -1) {
        float valorNulo = -1;
        fwrite(&valorNulo, sizeof(float), 1, arquivo);  // Escreve valor nulo
    } else {
        fwrite(&reg->tamanho, sizeof(float), 1, arquivo);  // Escreve o valor do tamanho
    }
    tamanhoUsado += sizeof(float);

    // Escreve o campo 'unidadeMedida', tratando valor nulo como '$'
    if (reg->unidadeMedida == '$') {
        fwrite(&trash, sizeof(char), 1, arquivo);  // Escreve o caractere de lixo
    } else {
        fwrite(&reg->unidadeMedida, sizeof(char), 1, arquivo);  // Escreve o valor da unidade de medida
    }
    tamanhoUsado += sizeof(char);

    // Escreve o campo 'velocidade', tratando valor nulo como -1
    if (reg->velocidade == -1) {
        int valorNulo = -1;
        fwrite(&valorNulo, sizeof(int), 1, arquivo);  // Escreve valor nulo
    } else {
        fwrite(&reg->velocidade, sizeof(int), 1, arquivo);  // Escreve o valor da velocidade
    }
    tamanhoUsado += sizeof(int);

    // Escreve o campo 'nome' (obrigatório, não pode ser nulo)
    for (int i = 0; reg->nome[i] != '\0'; i++) {
        fwrite(&reg->nome[i], sizeof(char), 1, arquivo);  // Escreve cada caractere da string
        tamanhoUsado += sizeof(char);
    }
    fwrite(&divider, sizeof(char), 1, arquivo);  // Escreve o delimitador após o campo
    tamanhoUsado += sizeof(char);

    // Escreve o campo 'especie' (obrigatório, não pode ser nulo)
    for (int i = 0; reg->especie[i] != '\0'; i++) {
        fwrite(&reg->especie[i], sizeof(char), 1, arquivo);  // Escreve cada caractere da string
        tamanhoUsado += sizeof(char);
    }
    fwrite(&divider, sizeof(char), 1, arquivo);  // Escreve o delimitador após o campo
    tamanhoUsado += sizeof(char);

    // Escreve o campo 'habitat', tratando valor nulo como delimitador
    if (reg->habitat[0] == '#') {
        fwrite(&divider, sizeof(char), 1, arquivo);  // Escreve o delimitador se for nulo
        tamanhoUsado += sizeof(char);
    } else {
        // Escreve cada caractere do campo habitat
        for (int i = 0; reg->habitat[i] != '\0'; i++) {
            fwrite(&reg->habitat[i], sizeof(char), 1, arquivo);
            tamanhoUsado += sizeof(char);
        }
        fwrite(&divider, sizeof(char), 1, arquivo);  // Escreve o delimitador após o campo
        tamanhoUsado += sizeof(char);
    }

    // Escreve o campo 'tipo', tratando valor nulo como delimitador
    if (reg->tipo[0] == '#') {
        fwrite(&divider, sizeof(char), 1, arquivo);  // Escreve o delimitador se for nulo
        tamanhoUsado += sizeof(char);
    } else {
        // Escreve cada caractere do campo tipo
        for (int i = 0; reg->tipo[i] != '\0'; i++) {
            fwrite(&reg->tipo[i], sizeof(char), 1, arquivo);
            tamanhoUsado += sizeof(char);
        }
        fwrite(&divider, sizeof(char), 1, arquivo);  // Escreve o delimitador após o campo
        tamanhoUsado += sizeof(char);
    }

    // Escreve o campo 'dieta' (obrigatório, não pode ser nulo)
    for (int i = 0; reg->dieta[i] != '\0'; i++) {
        fwrite(&reg->dieta[i], sizeof(char), 1, arquivo);  // Escreve cada caractere da string
        tamanhoUsado += sizeof(char);
    }
    fwrite(&divider, sizeof(char), 1, arquivo);  // Escreve o delimitador após o campo
    tamanhoUsado += sizeof(char);

    // Escreve o campo 'alimento', tratando valor nulo como delimitador
    if (reg->alimento[0] == '#') {
        fwrite(&divider, sizeof(char), 1, arquivo);  // Escreve o delimitador se for nulo
        tamanhoUsado += sizeof(char);
    } else {
        // Escreve cada caractere do campo alimento
        for (int i = 0; reg->alimento[i] != '\0'; i++) {
            fwrite(&reg->alimento[i], sizeof(char), 1, arquivo);
            tamanhoUsado += sizeof(char);
        }
        fwrite(&divider, sizeof(char), 1, arquivo);  // Escreve o delimitador após o campo
        tamanhoUsado += sizeof(char);
    }

    preencherComLixo(arquivo, tamanhoUsado);
}

// Função para ler um registro binário do arquivo e
Registro *lerRegistroBin(FILE *arquivo) {
    if (arquivo == NULL || feof(arquivo)) return NULL; // erro ou fim de arquivo

    Registro *r = criarRegistro(); // Cria o novo registro com valores padrões
    if (r == NULL) return NULL;  // Falha na criação

    // Salva a posição inicial do registro
    long int posInicial = ftell(arquivo);

    // Lê o campo "removido" (primeiro byte do registro)
    if (fread(&r->removido, sizeof(char), 1, arquivo) != 1) {
        liberarRegistro(r);
        return NULL;  // Chegou ao final do arquivo ou erro na leitura
    }

    // Se o registro foi logicamente removido (r->removido == '1')
    if (r->removido == '1') {
        // Pula os bytes restantes do registro (160 bytes no total)
        fseek(arquivo, posInicial + 160, SEEK_SET);
        liberarRegistro(r);  // Libera o registro
        return lerRegistroBin(arquivo);  // Tenta ler o próximo registro
    }

    // Lê os campos de tamanho fixo
    fread(&r->encadeamento, sizeof(int), 1, arquivo);
    fread(&r->populacao, sizeof(int), 1, arquivo);
    fread(&r->tamanho, sizeof(float), 1, arquivo);
    fread(&r->unidadeMedida, sizeof(char), 1, arquivo);
    fread(&r->velocidade, sizeof(int), 1, arquivo);


    // Lê as strings variáveis
    lerString(arquivo, r->nome);
    lerString(arquivo, r->especie);
    lerString(arquivo, r->habitat);
    lerString(arquivo, r->tipo);
    lerString(arquivo, r->dieta);
    lerString(arquivo, r->alimento);

    // Após ler o registro, pula-se para o próximo registro a partir da posição inicial
    long int endOfRecord = posInicial + 160;  // Calcula onde o próximo registro começa
    fseek(arquivo, endOfRecord, SEEK_SET);  // Pula o registro + lixo

    // Retorna o registro lido
    return r;
}

// Função para buscar registros com base em um campo e valor fornecidos
int buscarRegistro(Registro *r, const char *campo, const char *valor) {
    // Verifica se o campo é "nome" e compara o valor fornecido com o campo "nome" do registro
    if (strcmp(campo, "nome") == 0 && strcmp(r->nome, valor) == 0) return 1;

    // Verifica se o campo é "dieta" e compara o valor fornecido com o campo "dieta" do registro
    if (strcmp(campo, "dieta") == 0 && strcmp(r->dieta, valor) == 0) return 1;

    // Verifica se o campo é "habitat" e compara o valor fornecido com o campo "habitat" do registro
    if (strcmp(campo, "habitat") == 0 && strcmp(r->habitat, valor) == 0) return 1;

    // Verifica se o campo é "populacao" e compara o valor fornecido como número com o campo "populacao" do registro
    // A comparação é feita convertendo a string "valor" em um número inteiro
    if (strcmp(campo, "populacao") == 0 && atoi(valor) == r->populacao) return 1;

    // Verifica se o campo é "tipo" e compara o valor fornecido com o campo "tipo" do registro
    if (strcmp(campo, "tipo") == 0 && strcmp(r->tipo, valor) == 0) return 1;

    // Verifica se o campo é "velocidade" e compara o valor fornecido como número com o campo "velocidade" do registro
    if (strcmp(campo, "velocidade") == 0 && atoi(valor) == r->velocidade) return 1;

    // Verifica se o campo é "unidadeMedida" e compara o valor fornecido (primeiro caractere) com o campo "unidadeMedida"
    if (strcmp(campo, "unidadeMedida") == 0 && valor[0] == r->unidadeMedida) return 1;

    // Verifica se o campo é "tamanho" e compara o valor fornecido como número com o campo "tamanho" do registro
    // A comparação é feita convertendo a string "valor" em um número de ponto flutuante
    if (strcmp(campo, "tamanho") == 0 && atof(valor) == r->tamanho) return 1;

    // Verifica se o campo é "especie" e compara o valor fornecido com o campo "especie" do registro
    if (strcmp(campo, "especie") == 0 && strcmp(r->especie, valor) == 0) return 1;

    // Verifica se o campo é "alimento" e compara o valor fornecido com o campo "alimento" do registro
    if (strcmp(campo, "alimento") == 0 && strcmp(r->alimento, valor) == 0) return 1;

    // Se nenhum dos campos coincidir com o valor fornecido, retorna 0 (não encontrado)
    return 0;
}



// Função para marcar um registro como logicamente removido e reescrevê-lo no arquivo
void removeRegistro(Registro *r, FILE *arquivoBin, int topo, long int byteOffset, int *nroRegRem) {
    // Marca o registro como removido e atualiza o encadeamento
    r->removido = '1';  // Marca como logicamente removido
    r->encadeamento = topo;  // Define o próximo encadeado como o topo anterior

    fseek(arquivoBin, byteOffset - 160, SEEK_SET);  // Posiciona o ponteiro no início do registro
    fwrite(&r->removido, sizeof(char), 1, arquivoBin);  // Escreve o campo 'removido'
    fwrite(&r->encadeamento, sizeof(int), 1, arquivoBin);  // Escreve o campo 'encadeamento'

    // Preenche o restante do registro com "lixo"
    preencherComLixo(arquivoBin, (sizeof(char) + sizeof(int)));

    // Incrementa o número de registros removidos
    (*nroRegRem)++;
}


void compactarArquivoBinario(FILE *arquivoOriginal, FILE *arquivoCompactado) {
    Cabecalho header = inicializarCabecalho(); // Inicializa o cabeçalho

    // Lê o cabeçalho do arquivo original
    fread(&header, sizeof(Cabecalho), 1, arquivoOriginal);

    // Escreve o cabeçalho no arquivo compactado
    fwrite(&header, sizeof(Cabecalho), 1, arquivoCompactado);

    Registro *r = criarRegistro();

    // Ler e copiar todos os registros não removidos
    while ((r = lerRegistroBin(arquivoOriginal)) != NULL) {
        if (r->removido == '0') {
            // Copia o registro não removido para o novo arquivo
            fwrite(r, sizeof(Registro), 1, arquivoCompactado);
        }
        // Libera o registro corretamente após ser usado
        liberarRegistro(r);
    }

    // Atualiza o cabeçalho do novo arquivo com as informações de registros copiados
    header.nroRegRem = 0;  // Após a compactação, não há mais registros removidos
    header.nroPagDisco = (ftell(arquivoCompactado) + PAGE_SIZE - 1) / PAGE_SIZE;  // Atualiza as páginas de disco
    // Volta ao início do arquivo para reescrever o cabeçalho
    fseek(arquivoCompactado, 0, SEEK_SET);  
    fwrite(&header, sizeof(Cabecalho), 1, arquivoCompactado);
}


// Função para inserir um registro reutilizando registros logicamente removidos ou ao final do arquivo
void inserirRegistro(FILE *arquivo, Registro *r, Cabecalho *header) {
    // Se existirem registros logicamente removidos
    if (header->topo != -1) {
        // Reutiliza registros logicamente removidos
        int RRNRemovido = header->topo;  // RRN do topo da pilha de removidos
        long int byteOffset = PAGE_SIZE + (RRNRemovido * 160);  // Calcula o byte offset correto
        // Posiciona o ponteiro no arquivo para o registro removido
        fseek(arquivo, byteOffset, SEEK_SET);
        // Lê os unicos campos do registro removido (removido e encadeamento)
        char removido;
        int encadeamento;
        fread(&removido, sizeof(char), 1, arquivo);
        fread(&encadeamento, sizeof(int), 1, arquivo);
        
        // Atualiza o topo da pilha para o próximo registro removido
        header->topo = encadeamento; 
        
        // Reposiciona o ponteiro para o início do registro para reescrever o novo
        fseek(arquivo, byteOffset, SEEK_SET);

        // Marca o registro como válido (não removido)
        r->removido = '0';  
        r->encadeamento = -1;  // Não há encadeamento para um registro válido

        // Escreve o novo registro no lugar do registro removido
        escreverRegistro(arquivo, r);

        // Atualiza o número de registros removidos
        header->nroRegRem--;
        // O proxRRN NÃO deve ser atualizado aqui

    } else {
        // Caso não haja registros removidos, insere ao final do arquivo
        long int byteOffset = PAGE_SIZE + header->proxRRN * 160;  // Próximo byte offset
        fseek(arquivo, byteOffset, SEEK_SET);

        // Marca o registro como válido
        r->removido = '0';
        r->encadeamento = -1;

        // Escreve o novo registro no final do arquivo
        escreverRegistro(arquivo, r);

        header->proxRRN++; // Incrementa o valor do RRN
        
    }

}