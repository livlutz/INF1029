/*Lívia Lutz dos Santos - 2211055
Ana Luiza Pinto Marques - 2211960*/

#include <stdio.h>
#include <stdlib.h>
int main(void){
    float *rowsA, *rowsB;
    FILE *matrixA, *matrixB;

    matrixA = fopen("floats_256_2.0f.dat", "wb");
    matrixB = fopen("floats_256_5.0f.dat", "wb");

    rowsA = (float*)malloc((8 * 16)*sizeof(float));
    rowsB = (float*)malloc((8 * 16)*sizeof(float));

    if(rowsA == NULL || rowsB == NULL){
        printf("Erro ao alocar memoria\n");
        return 1;
    }

    for(int i = 0; i < (8 * 16);i++){
        rowsA[i] = 2.0;
        rowsB[i] = 5.0;
    }

    fwrite(rowsA, sizeof(float), (8 * 16), matrixA);
    fwrite(rowsB, sizeof(float), (8 * 16), matrixB);
    
    fclose(matrixA);
    fclose(matrixB);

    free(rowsA);
    free(rowsB);

    return 0;
}


/*Instruções para rodar o trabalho :

gcc -Wall -o gera_arquivo gera_arquivo.c
./gera_arquivo

No servidor da PUC : nvcc -o matrix_lib_test matrix_lib_test.cu matrix_lib.cu timer.c
 
Na minha humilde residencia : n sei ainda

matrix_lib_test 5.0 8 16 16 8 256 4096 1024 floats_256_2.0f.dat floats_256_5.0f.dat result1.dat result2.dat 


256 é o número de threads por bloco a serem disparadas;
4096 é o número máximo de blocos por GRID a serem usados;
1024 é a quantidade máxima de memória em MiB que pode ser alocado na GPGPU*/