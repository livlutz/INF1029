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

    cudaDeviceSynchronize();
    
    return 1;
}


/*Essa função recebe 3 matrizes como argumentos de entrada e calcula o valor do produto da
matriz A pela matriz B utilizando CUDA. Cada função kernel deve calcular o resultado
referente a um dos elementos da matriz C (ou mais de um elemento se o dataset for maior
que o número de threads do GRID). O resultado da operação deve ser retornado na matriz
C. Em caso de sucesso, a função deve retornar o valor 1. Em caso de erro, a função deve
retornar 0*/

__global__
void matrix_multiply(int matrixA_alloc_mode, unsigned long int matrixA_height, unsigned long int matrixA_width, unsigned long int matrixB_width, float *matrixA_rows, float *matrixB_rows, float *matrixC_rows) {

    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;    

    int indexC,linhaA,tamC,indexB;
    float result;

    tamC = matrixB_width * matrixA_height;

    if(matrixA_alloc_mode == 0){
        for (int i = index; i < tamC; i += stride) {
            matrixC_rows[i] += matrixA_rows[i] * matrixB_rows[i];
        }
    }

    else{

        for(int i = index; i < matrixA_height; i += stride){

            linhaA = i * matrixA_width;

            indexC = i * matrixB_width;

            for(int j = 0; j < matrixB_width; j++){

                result = 0; 

                for(int k = 0; k < matrixA_width; k++){

                    indexB = k * matrixB_width + j;
                    
                    result += matrixA_rows[linhaA + k] * matrixB_rows[indexB];
                }

                matrixC_rows[indexC + j] = result;
            }

        }
    }
}

int matrix_matrix_mult(struct matrix *matrixA, struct matrix * matrixB, struct matrix * matrixC){

    if((matrixA == NULL) || (matrixB == NULL) || (matrixC == NULL) || (matrixA->width != matrixB->height) || (matrixC->height != matrixA->height) || (matrixC->width != matrixB->width) ){
        printf("Erro de dimensao ou alocacao\n");
        return 0;
    }
    
    int A_alloc_mode;
    unsigned long int A_height,A_width,B_width;
    float *A_rows,*B_rows,*C_rows;

    A_alloc_mode = matrixA->alloc_mode;

    A_height = matrixA->height;
    A_width = matrixA->width; 
    B_width = matrixB->width; 

    A_rows = matrixA->d_rows;
    B_rows = matrixB->d_rows; 
    C_rows = matrixC->d_rows;

    if(A_alloc_mode == 0){

        for(int count = 0; count < A_height; count++){
            matrix_multiply<<<MAX_BLOCKS_PER_GRID, THREADS_PER_BLOCK>>>(A_alloc_mode, A_height, A_width, B_width, A_rows, B_rows, C_rows);
            cudaDeviceSynchronize();
        }

        return 1;
    }
    
    else {
        
        matrix_multiply<<<MAX_BLOCKS_PER_GRID, THREADS_PER_BLOCK>>>(A_alloc_mode, A_height, A_width, B_width, A_rows, B_rows, C_rows);
        cudaDeviceSynchronize();
        return 1;
    }

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