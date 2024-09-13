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
    __m256 scalar = _mm256_set1_ps(scalar_value);

   // Usar instruções AVX para multiplicar todos os elementos da matriz por um escalar
   for(int i = 0; i< matrix->height * matrix->width; i += 8){
        __m256 row = _mm256_load_ps(&matrix->rows[i]);
        __m256 result = _mm256_mul_ps(row,scalar);
        _mm256_store_ps(&matrix->rows[i],result);
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


   int indexA, indexB, indexC;

    for(int i = 0; i < matrixC->height; i++){
        //Calcula posicao inicial dos indices das matrizes A e C aqui e depois incrementa o valor dentro do loop
        indexA = i * matrixA->width;
        indexC = i * matrixC->width;
        // Ponteiro direto para a linha i da matriz C
        __m256 rowC = _mm256_load_ps(&matrixC->rows[indexC]);

        //itera por colunas da matriz A
        for(int j = 0; j < matrixA->width;j++){
            //Calcula posicao inicial dos indices da matriz B e depois incrementa o valor dentro do loop
            indexB = j * matrixB->width;

            //valor do elemento da matriz A
            __m256 valA = _mm256_set1_ps(matrixA->rows[indexA + j]);

            // Ponteiro direto para a linha j da matriz B
            __m256 rowB = _mm256_load_ps(&matrixB->rows[indexB]);

            //itera por colunas da matriz B
            for (int k = 0; k < matrixB->width; k++){
                //multiplica cada elemento da linha de A pelo elemento da coluna de B
                __m256 result = _mm256_fmadd_ps(rowB,valA,rowC);
                _mm256_store_ps(&matrixC->rows[indexC],result);
            }
        }
    }

    return 1;
}
