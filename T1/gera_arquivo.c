/*Lívia Lutz dos Santos - 2211055
Ana Luiza Pinto Marques - 2211960*/

#include "matrix_lib.h"

int main(void){
    float *rowsA, *rowsB;
    FILE *matrixA, *matrixB;

    matrixA = fopen("floats_256_2.0f.dat", "wb");
    matrixB = fopen("floats_256_5.0f.dat", "wb");

    rowsA = (float*)malloc((2048 * 2048)*sizeof(float));
    rowsB = (float*)malloc((2048 * 2048)*sizeof(float));

    if(rowsA == NULL || rowsB == NULL){
        printf("Erro ao alocar memoria\n");
        return 1;
    }

    for(int i = 0; i < (2048 * 2048);i++){
        rowsA[i] = 2.0;
        rowsB[i] = 5.0;
    }

    fwrite(rowsA, sizeof(float), (2048 * 2048), matrixA);
    fwrite(rowsB, sizeof(float), (2048 * 2048), matrixB);
    
    fclose(matrixA);
    fclose(matrixB);

    free(rowsA);
    free(rowsB);

    return 0;
}

/*Instrucoes pra rodar o trabalho : 
gcc -o matrix_lib_test matrix_lib_test.c matrix_lib.c timer.c
matrix_lib_test 5.0 2048 2048 2048 2048 floats_256_2.0f.dat floats_256_5.0f.dat result1.dat result2.dat*/
