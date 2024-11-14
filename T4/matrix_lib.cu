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

/*Cada função kernel deve calcular o
resultado do produto entre o valor escalar e um dos elementos da matriz (ou mais de um
elemento se o dataset for maior que o número de threads do GRID)*/
__global__
void scalar_mult(float scalar_value, float *d_rows, int matrix_size) {
    
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for(int i = index; i < matrix_size; i += stride) {
        d_rows[i] *= scalar_value;
    }
}


int scalar_matrix_mult(float scalar_value, struct matrix *matrix) {
    
    if(matrix == NULL) {
        return 0;
    }

    int matrix_size = matrix->height * matrix->width;
    int threads_per_block = THREADS_PER_BLOCK;
    int blocks_per_grid =(matrix_size + threads_per_block - 1) / threads_per_block;

    scalar_mult<<<blocks_per_grid, threads_per_block>>>(scalar_value, matrix->d_rows, matrix_size);
    
    return 1;
}


/*Essa função recebe 3 matrizes como argumentos de entrada e calcula o valor do produto da
matriz A pela matriz B utilizando CUDA. Cada função kernel deve calcular o resultado
referente a um dos elementos da matriz C (ou mais de um elemento se o dataset for maior
que o número de threads do GRID). O resultado da operação deve ser retornado na matriz
C. Em caso de sucesso, a função deve retornar o valor 1. Em caso de erro, a função deve
retornar 0*/

/*Cada função kernel deve calcular o resultado
referente a um dos elementos da matriz C (ou mais de um elemento se o dataset for maior
que o número de threads do GRID).*/

__global__
void matrix_multiply(float *d_rowsA, float *d_rowsB, float *d_rowsC, unsigned long int C_height, unsigned long int A_width, unsigned long int B_width, unsigned long int C_width) {

    int row = blockIdx.x * blockDim.x + threadIdx.x;
    int column = blockIdx.y * blockDim.y + threadIdx.y;

    int indexC,linhaA;
    float result;

    if (row < C_height && column < B_width) {
        result = 0.0f;
        
        linhaA = row * A_width;
        indexC = row * C_width + column;
        // A multiplicação de matrizes é feita na soma dos produtos A[i, k] * B[k, j]
        // Para a célula C[row, column], percorre toda a dimensão k (do tamanho de A_width)
        for (int k = 0; k < A_width; k++) {
            result += d_rowsA[linhaA + k] * d_rowsB[k * B_width + column];
        }
        
        // Armazenar o resultado final na matriz C
        d_rowsC[indexC] = result;
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

    // Configuração do número de threads por bloco
    dim3 blockSize(THREADS_PER_BLOCK, THREADS_PER_BLOCK);  // 16x16 threads por bloco

    // Calcular o número de blocos necessários em ambas as dimensões (x e y)
    dim3 numBlocks((C_width + blockSize.x - 1) / blockSize.x, (C_height + blockSize.y - 1) / blockSize.y);

    if(numBlocks.x > MAX_BLOCKS_PER_GRID || numBlocks.y > MAX_BLOCKS_PER_GRID){
        numBlocks.x = MAX_BLOCKS_PER_GRID;
        numBlocks.y = MAX_BLOCKS_PER_GRID;
        return 0;
    }

    // Chamar o kernel de multiplicação de matrizes
    matrix_multiply<<<numBlocks, blockSize>>>(d_rowsA, d_rowsB, d_rowsC, C_height, A_width, B_width, C_width);

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