/*Lívia Lutz dos Santos - 2211055
Ana Luiza Pinto Marques - 2211960*/

#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include <pthread.h>


/*Onde:
height = número de linhas da matriz (múltiplo de 8)
width = número de colunas da matriz (múltiplo de 8)
rows = sequência de linhas da matriz (height*width elementos)*/

struct matrix {
    unsigned long int height;
    unsigned long int width;
    float *rows;
};

int scalar_matrix_mult(float scalar_value, struct matrix *matrix);

int matrix_matrix_mult(struct matrix *matrixA, struct matrix * matrixB, struct matrix * matrixC);

void set_number_threads(int num_threads);