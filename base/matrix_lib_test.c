/*Lívia Lutz dos Santos - 2211055
Ana Luiza Pinto Marques - 2211960*/

#include "matrix_lib.h"
#include "timer.h"

/*O programa base principal deve cronometrar o tempo de execução geral do programa (overall time)
e o tempo de execução das funções scalar_matrix_mult e matrix_matrix_mult. */

int main(int argc, char *argv[]){

    //structs das matrizes
    struct matrix matrixA, matrixB, matrixC;

    //guarda o valor escalar
    float scalar_value;
    
    //ponteiros para os arquivos de inicializacao e saida
    FILE *A, *B, *multScalar, *multMatrix;

    //variaveis que guardam o retorno das funcoes
    int retorno_scalar,retorno_matrix;

    //variaveis para contagem de tempo
    struct timeval start, stop, overall_t1, overall_t2;

    // Mark overall start time
    gettimeofday(&overall_t1, NULL);

    //abre os arquivos de inicializacao das matrizes A, B e C para leitura e captura de dados
    A = fopen(argv[6], "rb");
    B = fopen(argv[7], "rb");
    
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

    //le os dados dos arquivos de entrada e inicializa as matrizes
    fread(matrixA.rows, sizeof(float), matrixA.height*matrixA.width,A);
    fread(matrixB.rows, sizeof(float), matrixB.height*matrixB.width,B);

    //inicializa a matrix C com 0
    for(int i = 0; i < (matrixA.height*matrixB.width); i++){
        matrixC.rows[i] = 0.0;
    }

    //a matriz C tem que ter o numero de linhas da matriz A e o numero de colunas da matriz B
    matrixC.height = matrixA.height;
    matrixC.width = matrixB.width;

    //fecha os arquivos de entrada
    fclose(A);
    fclose(B);

    //abre os arquivos de saida para escrita dos resultados
    multScalar = fopen(argv[8], "wb");
    multMatrix = fopen(argv[9], "wb");

    if(multScalar == NULL || multMatrix == NULL){
        printf("Erro ao abrir os arquivos\n");
        return 0;
    }

    // Mark scalar start time
    gettimeofday(&start, NULL);

    //chama as funcoes de multiplicacao por escalar
    retorno_scalar = scalar_matrix_mult(scalar_value, &matrixA);

    // Mark scalar stop time
    gettimeofday(&stop, NULL);

    // Mark matrix start time
    gettimeofday(&start, NULL);

    retorno_matrix = matrix_matrix_mult(&matrixA, &matrixB, &matrixC);

    // Mark matrix stop time
    gettimeofday(&stop, NULL);

    // Show matrix exec time
    printf("Matrix matrix multiplication time: %f ms\n", timedifference_msec(start, stop));
    
    //verifica se a multiplicacao foi feita corretamente
    if(retorno_scalar == 0){
        printf("Erro ao multiplicar as matrizes no scalar\n");
        return 0;
    }

    else if(retorno_matrix == 0){
        printf("Erro ao multiplicar as matrizes na matrix\n");
        return 0;
    }

    //escreve os resultados nos arquivos de saida
    fwrite(matrixA.rows, sizeof(float), matrixA.height*matrixA.width, multScalar);
    fwrite(matrixC.rows, sizeof(float), matrixA.height*matrixB.width, multMatrix);

    //fecha os arquivos de saida
    fclose(multScalar);
    fclose(multMatrix);

    //liberar a memoria das matrizes
    free(matrixA.rows);
    free(matrixB.rows);
    free(matrixC.rows);

    // Mark overall stop time
    gettimeofday(&overall_t2, NULL);
    
    // Show elapsed overall time
    printf("Overall time: %f ms\n", timedifference_msec(overall_t1, overall_t2));
    
    return 0;
}