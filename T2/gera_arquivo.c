/*Lívia Lutz dos Santos - 2211055
Ana Luiza Pinto Marques - 2211960*/

#include "matrix_lib.h"

int main(void){
    float *rowsA, *rowsB;
    FILE *matrixA, *matrixB;

    matrixA = fopen("floats_256_2.0f.dat", "wb");
    matrixB = fopen("floats_256_5.0f.dat", "wb");

    rowsA = (float*)malloc((1024 * 1024)*sizeof(float));
    rowsB = (float*)malloc((1024 * 1024)*sizeof(float));

    if(rowsA == NULL || rowsB == NULL){
        printf("Erro ao alocar memoria\n");
        return 1;
    }

    for(int i = 0; i < (1024 * 1024);i++){
        rowsA[i] = 2.0;
        rowsB[i] = 5.0;
    }

    fwrite(rowsA, sizeof(float), (1024 * 1024), matrixA);
    fwrite(rowsB, sizeof(float), (1024 * 1024), matrixB);
    
    fclose(matrixA);
    fclose(matrixB);

    free(rowsA);
    free(rowsB);

    return 0;
}

/*Instrucoes pra rodar o trabalho : 
No servidor da PUC :
gcc -std=c11 -mfma -o matrix_lib_test matrix_lib_test.c matrix_lib.c timer.c

Na minha humilde residencia :
gcc –mfma -mavx2 -o matrix_lib_test matrix_lib_test.c matrix_lib.c timer.c 

Exemplo de linha de comando:
./matrix_lib_test 5.0 1024 1024 1024 1024 floats_256_2.0f.dat floats_256_5.0f.dat result1.dat result2.dat
Onde,
5.0 é o valor escalar que multiplicará a primeira matriz;
8 é o número de linhas da primeira matriz;
16 é o número de colunas da primeira matriz;
16 é o número de linhas da segunda matriz;
8 é o número de colunas da segunda matriz;
floats_256_2.0f.dat é o nome do arquivo de floats que será usado para carregar a primeira matriz;
floats_256_5.0f.dat é o nome do arquivo de floats que será usado para carregar a segunda matriz;
result1.dat é o nome do arquivo de floats onde o primeiro resultado será armazenado;
result2.dat é o nome do arquivo de floats onde o segundo resultado será armazenado.

Como rodar o gera arquivo no servidor da PUC :
gcc -std=c11 -Wall -o gera_arquivo gera_arquivo.c
./gera_arquivo*/