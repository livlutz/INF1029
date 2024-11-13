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

    cudaError_t cudaError;
    int matrix_size = matrix->height * matrix->width;
    int threads_per_block = THREADS_PER_BLOCK;
    int blocks_per_grid =(matrix_size + threads_per_block - 1) / threads_per_block;

    if(matrix->alloc_mode == 0){
        for(int i = 0; i < matrix->height; i++){
            cudaError = cudaMemcpy(matrix->d_rows, &matrix->h_rows[i * matrix->width], matrix->width * sizeof(float), cudaMemcpyHostToDevice);
            if (cudaError != cudaSuccess) {
                printf("cudaMemcpy h_rowA -> d_rowA returned error %s (code %d)\n", cudaGetErrorString(cudaError), cudaError);
                cudaFree(matrix->d_rows);
                return 0;
            }

            scalar_mult<<<blocks_per_grid, threads_per_block>>>(scalar_value, matrix->d_rows, matrix_size);
            cudaDeviceSynchronize();

            cudaError = cudaMemcpy(&matrix->h_rows[i * matrix->width], matrix->d_rows, matrix->width * sizeof(float), cudaMemcpyDeviceToHost);
            if (cudaError != cudaSuccess) {
                printf("cudaMemcpy d_rowA -> h_rowA returned error %s (code %d)\n", cudaGetErrorString(cudaError), cudaError);
                cudaFree(matrix->d_rows);
                return 0;
            }
        }

        return 1;
    }

    else{
        scalar_mult<<<blocks_per_grid, threads_per_block>>>(scalar_value, matrix->d_rows, matrix_size);
        cudaDeviceSynchronize();
        return 1;
    }
   
    return 0;
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

    int indexA, indexB, indexC;
    float valA,rowB,rowC,result;

    //itera por linhas da matriz C
    for (int i = index; i < C_height; i += stride) {

        //itera por colunas da matriz A
        for (int j = 0; j < A_width; j++) {

            //Calcula posicao inicial do indice da matrizA 
            indexA = i * A_width + j;

            //valor do elemento da matriz A
            valA = d_rowsA[indexA];

            //itera por linhas da matriz B
            for (int k = 0; k < B_width; k++) {

                //Calcula posicao inicial do indice da matrizB
                indexB = j * B_width + k;
                //Calcula posicao inicial dos indices da matrizC aqui e depois incrementa o valor dentro do loop
                indexC = i * C_width + k;

                rowB = d_rowsB[indexB];

                rowC = d_rowsC[indexC];

                //Calcula o valor do elemento da matriz C
                result = rowC + valA * rowB;

                //Atualiza o valor do elemento da matriz C
                d_rowsC[indexC] = result;

            }

        }
    }
}

int matrix_matrix_mult(struct matrix *matrixA, struct matrix * matrixB, struct matrix * matrixC){

    if((matrixA == NULL) || (matrixB == NULL) || (matrixC == NULL) || (matrixA->width != matrixB->height) || (matrixC->height != matrixA->height) || (matrixC->width != matrixB->width) ){
        printf("Erro de dimensao ou alocacao\n");
        return 0;
    }

    cudaError_t cudaError;
    float *d_rowsA = matrixA->d_rows;
    float *d_rowsB = matrixB->d_rows;
    float *d_rowsC = matrixC->d_rows;
    unsigned long int C_height = matrixC->height;
    unsigned long int A_width = matrixA->width;
    unsigned long int B_width = matrixB->width;
    unsigned long int C_width = matrixC->width;

    int blockSize = THREADS_PER_BLOCK;
    int numBlocks = ((matrixC->height * matrixC->width) + blockSize - 1) / blockSize;
    if (numBlocks > MAX_BLOCKS_PER_GRID) numBlocks = MAX_BLOCKS_PER_GRID;

    if(matrixA->alloc_mode == 0 && matrixC->alloc_mode == 0){
        // Processa cada linha de A e C individualmente
        for (int i = 0; i < matrixA->height; i++) {
            // Copia a linha atual de A para o device
            cudaError = cudaMemcpy(d_rowsA, &matrixA->h_rows[i * matrixA->width], matrixA->width * sizeof(float), cudaMemcpyHostToDevice);
            if (cudaError != cudaSuccess) {
                printf("cudaMemcpy h_rowA -> d_rowA returned error %s (code %d)\n", cudaGetErrorString(cudaError), cudaError);
                cudaFree(d_rowsA);
                cudaFree(d_rowsC);
                return 0;
            }

            // Copia a linha atual de C para o device
            cudaError = cudaMemcpy(d_rowsC, &matrixC->h_rows[i * matrixC->width], matrixC->width * sizeof(float), cudaMemcpyHostToDevice);
            if (cudaError != cudaSuccess) {
                printf("cudaMemcpy h_rowC -> d_rowC returned error %s (code %d)\n", cudaGetErrorString(cudaError), cudaError);
                cudaFree(d_rowsA);
                cudaFree(d_rowsC);
                return 0;
            }

            // Executa o kernel para calcular a linha `i` de C
            matrix_multiply<<<numBlocks, blockSize>>>(d_rowsA, matrixB->d_rows, d_rowsC, 1, matrixA->width, matrixB->width, matrixC->width);
            cudaDeviceSynchronize();

            // Copia o resultado da linha `i` de C do device de volta para a host
            cudaError = cudaMemcpy(&matrixC->h_rows[i * matrixC->width], d_rowsC, matrixC->width * sizeof(float), cudaMemcpyDeviceToHost);
            if (cudaError != cudaSuccess) {
                printf("cudaMemcpy d_rowC -> h_rowC returned error %s (code %d)\n", cudaGetErrorString(cudaError), cudaError);
                cudaFree(d_rowsA);
                cudaFree(d_rowsC);
                return 0;
            }

            // Copia a linha atual de A de volta para a host
            cudaError = cudaMemcpy(&matrixA->h_rows[i * matrixA->width], d_rowsA, matrixA->width * sizeof(float), cudaMemcpyDeviceToHost);
            if (cudaError != cudaSuccess) {
                printf("cudaMemcpy d_rowA -> h_rowA returned error %s (code %d)\n", cudaGetErrorString(cudaError), cudaError);
                cudaFree(d_rowsA);
                cudaFree(d_rowsC);
                return 0;
            }
        }
    }

    else{
        matrix_multiply<<<numBlocks,blockSize>>>(d_rowsA,d_rowsB,d_rowsC,C_height,A_width,B_width,C_width);
        cudaDeviceSynchronize();
        return 1;
    }

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