/*Lívia Lutz dos Santos - 2211055
Ana Luiza Pinto Marques - 2211960*/

#include "matrix_lib.h"
#include "timer.h"

static int THREADS_PER_BLOCK, MAX_BLOCKS_PER_GRID;

/*Essa função recebe um valor escalar e uma matriz como argumentos de entrada e calcula o
produto do valor escalar pela matriz utilizando CUDA. Cada função kernel deve calcular o
resultado do produto entre o valor escalar e um dos elementos da matriz (ou mais de um
elemento se o dataset for maior que o número de threads do GRID). O resultado da
operação deve ser retornado na matriz de entrada. Em caso de sucesso, a função deve
retornar o valor 1. Em caso de erro, a função deve retornar 0.*/
__global__
void scalar_mult(float scalar_value, float *d_rows, int matrix_size) {
    
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    if(index == 0){
        printf("\nblockDim.x=%d   gridDim.x=%d    stride=%d\n",blockDim.x,gridDim.x,stride);
    }

    for(int i = index; i < matrix_size; i += stride) {
        d_rows[i] *= scalar_value;
    }
}


int scalar_matrix_mult(float scalar_value, struct matrix *matrix) {
    if(matrix == NULL) {
        return 0;
    }

    int matrix_size = matrix->height * matrix->width;
    scalar_mult<<<MAX_BLOCKS_PER_GRID, THREADS_PER_BLOCK>>>(scalar_value, matrix->d_rows, matrix_size);

    return 1;
}


/*Essa função recebe 3 matrizes como argumentos de entrada e calcula o valor do produto da
matriz A pela matriz B utilizando CUDA. Cada função kernel deve calcular o resultado
referente a um dos elementos da matriz C (ou mais de um elemento se o dataset for maior
que o número de threads do GRID). O resultado da operação deve ser retornado na matriz
C. Em caso de sucesso, a função deve retornar o valor 1. Em caso de erro, a função deve
retornar 0*/

__global__
void matrix_multiply(struct matrix *matrixA, struct matrix *matrixB, struct matrix *matrixC){
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    //Calcular o indice da matrix C pra cada thread

    // Debug para exibir valores iniciais
    if(index == 0){
      printf("\nblockDim.x=%d   gridDim.x=%d    stride=%d\n", blockDim.x, gridDim.x, stride);
    }

    float sum;
    int indexA, indexB, indexC;
    float *d_rowsA = matrixA->d_rows;
    float *d_rowsB = matrixB->d_rows;
    float *d_rowsC = matrixC->d_rows;

    // Calcular a multiplicação por bloco e por thread, onde cada thread cuida de um elemento de matrixC
    for (int i = index; i < matrixC->height; i += stride) {
        for (int k = 0; k < matrixC->width; k++) {
            sum = 0.0f;

            for (int j = 0; j < matrixA->width; j++) {
                indexA = i * matrixA->width + j;
                indexB = j * matrixB->width + k;

                // Acumula o produto de A e B
                sum += d_rowsA[indexA] * d_rowsB[indexB];
            }

            // Armazena o valor final na posição (i, k) de C
            indexC = i * matrixC->width + k;
            d_rowsC[indexC] = sum;
        }
    }
}

int matrix_matrix_mult(struct matrix *matrixA, struct matrix * matrixB, struct matrix * matrixC){

    if((matrixA == NULL) || (matrixB == NULL) || (matrixC == NULL) || (matrixA->width != matrixB->height) || (matrixC->height != matrixA->height) || (matrixC->width != matrixB->width) ){
        printf("Erro de dimensao ou alocacao\n");
        return 0;
    }

    matrix_multiply<<<MAX_BLOCKS_PER_GRID,THREADS_PER_BLOCK>>>(matrixA,matrixB,matrixC);
   
    return 1;
}

/*Essa função recebe o número de threads por bloco e o número máximo de blocos por grid
que devem ser usados como parâmetros para disparar os threads (funções kernel) em
paralelo durante o processamento das operações aritméticas com as matrizes e deve ser
chamada pelo programa principal antes das outras funções*/
/*1024 para o número de
threads por bloco e 65535 para o número de blocos por grid. Os valores limites para a
GPGPU NVIDIA GeForce RTX 4070 Ti são 1024 para o número de threads por bloco e
2147483647 para o número de blocos por grid.*/

int set_grid_size(int threads_per_block, int max_blocks_per_grid){

    // tem q saber qual é a gpu sendo usada pra testar o max!!
    if((threads_per_block > 1024) && (max_blocks_per_grid > 2147483647)){
        return 0;
    }

    THREADS_PER_BLOCK = threads_per_block;
    MAX_BLOCKS_PER_GRID = max_blocks_per_grid;

    return 1;
}