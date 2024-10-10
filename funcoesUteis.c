
#include <ctype.h>
#include <string.h>
#include "funcoesFornecidas.h"
#include "cabecalho.h"

#define DELIMITER '#'
#define TRASH '$'

// Função auxiliar para ler strings delimitadas e tratar o lixo
void lerString(FILE *arquivo, char *dest) {
    char ch;  // Variável para armazenar temporariamente cada caractere lido
    int i = 0;  // Índice para acompanhar a posição na string de destino

    // Loop infinito para ler caracteres até encontrar o delimitador ou o lixo
    while (1) {
        fread(&ch, sizeof(char), 1, arquivo);  // Lê um caractere do arquivo

        // Se o caractere for o delimitador (DELIMITER = '#'), é o fim da string
        if (ch == DELIMITER) {
            dest[i] = '\0';  // Adiciona o caractere nulo para terminar a string
            break;  // Sai do loop
        }

        // Se o caractere for o lixo (TRASH = '$'), pula ele e não adiciona à string
        if (ch == TRASH) {
            continue;  // Ignora o lixo e passa para a próxima iteração
        }

        // Se não for nem delimitador nem lixo, adiciona o caractere à string de destino
        dest[i++] = ch;
    }
}



// Função auxiliar para preencher o restante do registro com "lixo"
void preencherComLixo(FILE *arquivoBin , int tamanhoUsado) {
    char trash = TRASH;
    // Preenche o restante do registro até 160 bytes com o caractere de lixo '$'
    int espacoRestante = 160 - tamanhoUsado;  // Calcula o espaço restante no registro
    for (int i = 0; i < espacoRestante; i++) {
        fwrite(&trash, sizeof(char), 1, arquivoBin);  // Preenche com o caractere de lixo
    }
    
}
