/*Lívia Lutz dos Santos - 2211055
Ana Luiza Pinto Marques - 2211960*/

#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include <pthread.h>


/*Onde:
height = número de linhas da matriz (múltiplo de 8)
width = número de colunas da matriz (múltiplo de 8)
h_rows = sequência de linhas da matriz (height*width elementos alocados no host)
d_rows = sequência de linhas da matriz (height*width elementos alocados no device)
alloc_mode = FULL_ALLOC (valor inteiro 1) ou PARTIAL_ALLOC (valor inteiro 0)*/

struct matrix {
    unsigned long int height;
    unsigned long int width;
    float *h_rows;
    float *d_rows;
    int alloc_mode;
};

int scalar_matrix_mult(float scalar_value, struct matrix *matrix);

int matrix_matrix_mult(struct matrix *matrixA, struct matrix * matrixB, struct matrix * matrixC);

void* scalar_matrix_thread(void* threadarg);

void* matrix_matrix_mult_thread(void* threadarg);

int set_grid_size(int threads_per_block, int max_blocks_per_grid);