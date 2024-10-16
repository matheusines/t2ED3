#include <stdio.h>         
#include <stdlib.h>        
#include "funcionalidades.h" 


/* 

___________________________________________________
|                                                  |
|. Bruno Tomé Rosa (Nº USP: 10276654)              |
|    -> Atribuições: funcionalidades 7 e 9         |
|    -> Conclusão: 100%                            |
|                                                  |
|. Matheus dos Santos Inês (Nº USP: 12546784)      |
|    -> Atribuições: funcionalidades 8             |
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


/*

**************************************************************************************************************************************
*                                                                                                                                    *
*                                                             TRABALHO 2                                                             *
*                                                                                                                                    *
**************************************************************************************************************************************

*/


        case 7:                    // Caso o usuário insira 7
            funcionalidade7();     // Chama a função `funcionalidade7()`
            break;

        case 8:                    // Caso o usuário insira 8
            funcionalidade8();     // Chama a função `funcionalidade8()`
            break;


        case 9:                    // Caso o usuário insira 9
            funcionalidade9();     // Chama a função `funcionalidade9()`
            break;
        default:                   // Caso o usuário insira qualquer outro valor que não esteja entre 1 e 9
            break;                 // Nenhuma ação é realizada.
    }
}