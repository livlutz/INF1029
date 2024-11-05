/*Lívia Lutz dos Santos - 2211055
Ana Luiza Pinto Marques - 2211960*/

#include "matrix_lib.h"
#include "timer.h"

static THREADS_PER_BLOCK, MAX_BLOCKS_PER_GRID

/*Essa função recebe um valor escalar e uma matriz como argumentos de entrada e calcula o
produto do valor escalar pela matriz. O resultado da operação deve ser retornado na matriz
de entrada. Em caso de sucesso, a função deve retornar o valor 1. Em caso de erro, a
função deve retornar 0*/

int scalar_matrix_mult(float scalar_value, struct matrix *matrix){
    
    if(matrix == NULL){
        return 0;
    }

    //verificar isso !!

    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.d_x;

    if(index == 0){
      printf("\nblockDim.x=%d   gridDim.x=%d    stride=%d\n",blockDim.x,gridDim.x,stride);
    }

    for (int i = index; i < n; i += stride) {
        matrix.d_rows[i] *= scalar_value;
    }

    return 1;

}


/*Essa função recebe 3 matrizes como argumentos de entrada e calcula o valor do produto da
matriz A pela matriz B. O resultado da operação deve ser retornado na matriz C. Em caso
de sucesso, a função deve retornar o valor 1. Em caso de erro, a função deve retornar 0.*/

int matrix_matrix_mult(struct matrix *matrixA, struct matrix * matrixB, struct matrix * matrixC){

    if((matrixA == NULL) || (matrixB == NULL) || (matrixC == NULL) || (matrixA->width != matrixB->height) || (matrixC->height != matrixA->height) || (matrixC->width != matrixB->width) ){
        printf("Erro de dimensao ou alocacao\n");
        return 0;
    }

    for (int i = linha_inicio; i < linha_fim; i++) {  // i itera sobre as linhas da matriz C
        indexC = i * c_width; 
        indexA = i * a_width;                       // Índice base da linha i de C
        c_row = &c_rows[indexC];                 // Ponteiro direto para a linha i de C
        a_row = &a_rows[indexA];                 // Ponteiro direto para a linha i de A

        for (int j = 0; j < a_width; j++) {           // Itera sobre as colunas da matriz A
            valA = _mm256_set1_ps(a_row[j]);          // Carrega o valor de A para multiplicar com a linha de B
            indexB = j * b_width;
            b_row = &b_rows[indexB];             // Ponteiro direto para a linha j de B

            for (int k = 0; k < b_width; k += 8) {    // Itera sobre as colunas da matriz B e processa 8 elementos por vez
                // Carrega 8 elementos de B e C para o cálculo 
                rowB = _mm256_load_ps(&b_row[k]);     // Carrega 8 elementos da linha de B
                rowC = _mm256_load_ps(&c_row[k]);     // Carrega 8 elementos da linha de C

                // Multiplica cada elemento da linha de A pelo elemento correspondente da coluna de B e acumula em C
                result = _mm256_fmadd_ps(rowB, valA, rowC);

                // Armazena o resultado na linha i de C
                _mm256_store_ps(&c_row[k], result);
            }
        }
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

    // tem q saber qual é a gpu sendo usada pra testar o max!!
    if((threads_per_block > 1024) && (max_blocks_per_grid > 65535 || max_blocks_per_grid > 2147483647)){
        return 0;
    }

    THREADS_PER_BLOCK = threads_per_block;
    MAX_BLOCKS_PER_GRID = max_blocks_per_grid;

    return 1;
}