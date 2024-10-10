#include <stdio.h>         
#include <stdlib.h>        
#include "funcionalidades.h" // Inclui o arquivo de cabeçalho "funcionalidades.h" onde as funções `funcionalidade1()`, `funcionalidade2()`, etc., estão declaradas.

/* 

___________________________________________________
|                                                  |
|. Bruno Tomé Rosa (Nº USP: 10276654)              |
|    -> Atribuições: funcionalidades 2, 4, 6       |
|    -> Conclusão: 100%                            |
|                                                  |
|. Matheus dos Santos Inês (Nº USP: 12546784)      |
|    -> Atribuições: funcionalidades 1, 3, 5       |
|    -> Conclusão: 100%                            |
|                                                  |
|__________________________________________________|

*/

int main() {
    int funcionalidade;              // Declara a variável 'funcionalidade' para armazenar a opção do usuário.
    scanf("%d", &funcionalidade);    // Lê a entrada do usuário e armazena o valor na variável 'funcionalidade'.

    // Estrutura switch para executar a função correspondente com base na entrada do usuário.
    switch (funcionalidade) {
        case 1:                    // Caso o usuário insira 1
            funcionalidade1();     // Chama a função `funcionalidade1()`
            break;                 

        case 2:                    // Caso o usuário insira 2
            funcionalidade2();     // Chama a função `funcionalidade2()`
            break;                 

        case 3:                    // Caso o usuário insira 3
            funcionalidade3();     // Chama a função `funcionalidade3()`
            break;                 
            
        case 4:                    // Caso o usuário insira 4
            funcionalidade4();     // Chama a função `funcionalidade4()`
            break;                 

        case 5:                    // Caso o usuário insira 5
            funcionalidade5();     // Chama a função `funcionalidade5()`
            break;                 

        case 6:                    // Caso o usuário insira 6
            funcionalidade6();     // Chama a função `funcionalidade6()`
            break;                 

        default:                   // Caso o usuário insira qualquer outro valor que não esteja entre 1 e 6
            break;                 // Nenhuma ação é realizada.
    }
}