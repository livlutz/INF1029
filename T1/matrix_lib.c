/*Lívia Lutz dos Santos - 2211055
Ana Luiza Pinto Marques - 2211960*/

#include "matrix_lib.h"
#include "timer.h"

/*Essa função recebe um valor escalar e uma matriz como argumentos de entrada e calcula o
produto do valor escalar pela matriz. O resultado da operação deve ser retornado na matriz
de entrada. Em caso de sucesso, a função deve retornar o valor 1. Em caso de erro, a
função deve retornar 0*/

int scalar_matrix_mult(float scalar_value, struct matrix *matrix){
    
    //se o ponteiro da matrix for nulo (matrix nao existe), retorna erro
    if (matrix == NULL){
        return 0;
    }

    //itera por linhas da matriz
    for(int i = 0; i < matrix->height;i++){
        //itera por colunas da matriz
        for(int j = 0; j < matrix->width;j++){
            /*calcula o multiplo de 8 da posicao do elemento da matrix no array de rows 
            e multiplica pelo escalar*/
            matrix->rows[i * matrix->width + j] *= scalar_value;
        }
    }

    return 1;
}

/*Essa função recebe 3 matrizes como argumentos de entrada e calcula o valor do produto da
matriz A pela matriz B. O resultado da operação deve ser retornado na matriz C. Em caso
de sucesso, a função deve retornar o valor 1. Em caso de erro, a função deve retornar 0.*/

int matrix_matrix_mult(struct matrix *matrixA, struct matrix * matrixB, struct matrix * matrixC){

    /*se o ponteiro de alguma matrix for nulo (matrix nao existe), retorna erro
    o numero de colunas da primeira matrix tem q ser igual ao numero de linhas da segunda matrix
    a matriz resultante tem que ter o numero de linhas da primeira matriz e o numero de colunas da segunda matriz*/

    if((matrixA == NULL) || (matrixB == NULL) || (matrixC == NULL) || (matrixA->width != matrixB->height) || (matrixC->height != matrixA->height) || (matrixC->width != matrixB->width) ){
        printf("Erro de dimensao ou alocacao\n");
        return 0;
    }

    int cont = 0;
    for(int i = 0; i < matrixA->height; i++){
        for(int j = 0; j < matrixB->height;j++){
            for (int k = 0; k < matrixB->width; k++){
                //multiplica cada elemento da linha de A pelo elemento da coluna de B
                matrixC->rows[i * matrixA->height + k] += matrixA->rows[i * matrixA->width + j] * matrixB->rows[j * matrixB->width + k];
            }
        }
    }

    return 1;

}
