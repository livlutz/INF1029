#include "matrix_lib.h"
#include "timer.h"

int main(int argc, char *argv[]){

    //structs das matrizes
    struct matrix matrixA, matrixB, matrixC;

    //guarda o valor escalar
    float scalar_value;
    
    //ponteiros para os arquivos de inicializacao e saida
    FILE *A, *B, *multScalar, *multMatrix;

    //abre os arquivos de inicializacao das matrizes A, B e C para leitura e captura de dados
    A = fopen(argv[6], "r");
    B = fopen(argv[7], "r");
    
    //verifica se o arquivo é nulo
    if(A == NULL || B == NULL){
        printf("Erro ao abrir os arquivos\n");
        return 0;
    }

    //le o valor escalar da linha de comando
    scalar_value = atof(argv[1]);
    
    //leitura dos valores de altura e largura das matrizes A e B da linha de comando
    matrixA.height = atoi(argv[2]);
    matrixA.width = atoi(argv[3]);
    matrixB.height = atoi(argv[4]);
    matrixB.width = atoi(argv[5]);

    //alocacao de memoria para as matrizes A, B e C
    matrixA.rows = (float*) malloc( (matrixA.height * matrixA.width) * sizeof(float));
    matrixB.rows = (float*) malloc( (matrixB.height * matrixB.width) * sizeof(float));
    matrixC.rows = (float*) malloc( (matrixA.height * matrixB.width) * sizeof(float));

    //verifica se a matriz foi alocada corretamente
    if(matrixA.rows == NULL || matrixB.rows == NULL || matrixC.rows == NULL){
        printf("Erro ao alocar memoria\n");
        return 0;
    }

    //le os dados dos arquivos de entrada
    fread(matrixA.rows, sizeof(float), matrixA.height*matrixA.width,A);
    fread(matrixB.rows, sizeof(float), matrixB.height*matrixB.width,B);

    fclose(A);
    fclose(B);

    //abre os arquivos de saida para escrita dos resultados
    multScalar = fopen(argv[8], "w");
    multMatrix = fopen(argv[9], "w");

    if(multScalar == NULL || multMatrix == NULL){
        printf("Erro ao abrir os arquivos\n");
        return 0;
    }
    
    //chamar as funcoes e escrever
    int retorno_scalar = scalar_matrix_mult(scalar_value, &matrixA);
    int retorno_matrix = matrix_matrix_mult(&matrixA, &matrixB, &matrixC);
    
    //verifica se a multiplicacao foi feita corretamente
    if(retorno_scalar == 0 || retorno_matrix == 0){
        printf("Erro ao multiplicar as matrizes\n");
        return 0;
    }

    //escreve os resultados nos arquivos de saida
    fwrite(matrixA.rows, sizeof(float), matrixA.height*matrixA.width, multScalar);
    fwrite(matrixC.rows, sizeof(float), matrixA.height*matrixB.width, multMatrix);

    fclose(multScalar);
    fclose(multMatrix);

    //liberar a memoria das matrizes
    free(matrixA.rows);
    free(matrixB.rows);
    free(matrixC.rows);
    
    return 0;
}

/*matrix_lib_test 5.0 8 16 16 8 floats_256_2.0f.dat floats_256_5.0f.dat result1.dat result2.dat*/

// matrix_lib_test chamada do programa

// 5.0 valor escalar

// 8 numero de linhas da matriz A e numero de colunas da matriz B

// 16 numero de colunas da matriz A e numero de linhas da matriz B

// 8 numero de linhas da matriz C e numero de colunas da matriz C

// floats_256_2.0f.dat arquivo de entrada da matriz A

// floats_256_5.0f.dat arquivo de entrada da matriz B

// result1.dat arquivo de saida da scalar

// result2.dat arquivo de saida da matrix c de multiplicacao