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
void matrix_multiply(float *d_rowsA, float *d_rowsB, float *d_rowsC, unsigned long int C_height, unsigned long int A_width, unsigned long int B_width, unsigned long int C_width) {

    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    float sum;
    int indexA, indexB, indexC;

    // Calcular a multiplicação por bloco e por thread, onde cada thread cuida de um elemento de matrixC
    for (int i = index; i < C_height; i += stride) {
        for (int k = 0; k < A_width; k++) {
            sum = 0.0f;

            for (int j = 0; j < B_width; j++) {
                indexA = i * A_width + j;
                indexB = j * B_width + k;

                // Acumula o produto de A e B
                sum += d_rowsA[indexA] * d_rowsB[indexB];
            }

            // Armazena o valor final na posição (i, k) de C
            indexC = i * C_width+ k;
            d_rowsC[indexC] = sum;
        }
    }
}

int matrix_matrix_mult(struct matrix *matrixA, struct matrix * matrixB, struct matrix * matrixC){

    if((matrixA == NULL) || (matrixB == NULL) || (matrixC == NULL) || (matrixA->width != matrixB->height) || (matrixC->height != matrixA->height) || (matrixC->width != matrixB->width) ){
        printf("Erro de dimensao ou alocacao\n");
        return 0;
    }

    float *d_rowsA = matrixA->d_rows;
    float *d_rowsB = matrixB->d_rows;
    float *d_rowsC = matrixC->d_rows;
    unsigned long int C_height = matrixC->height;
    unsigned long int A_width = matrixA->width;
    unsigned long int B_width = matrixB->width;
    unsigned long int C_width = matrixC->width;

    matrix_multiply<<<MAX_BLOCKS_PER_GRID,THREADS_PER_BLOCK>>>(d_rowsA,d_rowsB,d_rowsC,C_height,A_width,B_width,C_width);
   
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

    if((threads_per_block > 1024) && (max_blocks_per_grid > 2147483647)){
        return 0;
    }

    THREADS_PER_BLOCK = threads_per_block;
    MAX_BLOCKS_PER_GRID = max_blocks_per_grid;

    return 1;
}