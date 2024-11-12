/*Lívia Lutz dos Santos - 2211055
Ana Luiza Pinto Marques - 2211960*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <cuda_runtime.h>
extern "C" {
#include "timer.h"
}
#include "matrix_lib.h"

float scalar_value = 0.0f;

struct matrix matrixA, matrixB, matrixC;


int store_matrix(struct matrix *matrix, char *filename) {
    FILE* arq = fopen(filename, "wb");

    if(arq == NULL){
        printf("Erro ao abrir o arquivo para escrita\n");
        return 0;
    }
    int qtd = fwrite(matrix->h_rows, sizeof(float), matrix->height*matrix->width, arq);

    if(qtd != matrix->height*matrix->width){
        printf("Erro ao escrever no arquivo\n");
        return 0;
    }

    fclose(arq);

    return 1;
}

int load_matrix(struct matrix *matrix, char *filename) {
    FILE* arq = fopen(filename, "rb");

    if(arq == NULL){
        printf("Erro ao abrir o arquivo\n");
        return 0;
    }

    int qtd = fread(matrix->h_rows, sizeof(float), matrix->height*matrix->width, arq);

    if(qtd != matrix->height*matrix->width){
        printf("Erro ao ler o arquivo\n");
        return 0;
    }

    fclose(arq);

    return 1;
}

int initialize_matrix(struct matrix *matrix, float value, float inc) {
    int ind;
    for(int i = 0; i < matrix->height; i++){
        for(int j = 0; j < matrix->width; j++){
            ind = i * matrix->height + j;
            matrix->h_rows[ind] = value;
            value += inc;
        }
    }

    return 1;
}

int print_matrix(struct matrix *matrix) {
    for(int i = 0; i < matrix->height; i++){
        for(int j = 0; j < matrix->width; j++){
	        if((i * matrix->height + j) > 256){
		        printf("Ooops...256 printing limit found...skipping printing...\n");
		        return 1;
	        }	
            printf("%f ", matrix->h_rows[i * matrix->height + j]);
        }
        printf("\n");
    }

    return 1;
}

int check_errors(struct matrix *matrix, float scalar_value) {
    for(int i = 0; i < matrix->height; i++){
        for(int j = 0; j < matrix->width; j++){
            if(matrix->h_rows[i * matrix->height + j] != scalar_value){
                printf("Matrix error\nExpected value : %f\nReceived value:%f\n", scalar_value, matrix->h_rows[i * matrix->height + j]);
                return 0;
            }
        }
    }

    return 1;
}

int main(int argc, char *argv[]) {
    char *result1_filename, *result2_filename;
    char *eptr = NULL;
    struct timeval start, stop, overall_t1, overall_t2;
    int carregaA, carregaB, inicializaC,max_mem_gpu,threads_per_block,max_blocks_per_grid,somaTotalMemMatriz,somaTotalMemB;
    cudaError_t cudaError;

    // Mark overall start time
    gettimeofday(&overall_t1, NULL);

    // Disable buffering entirely
    setbuf(stdout, NULL);

    // Check arguments
    if (argc != 13) {
            printf("Usage: %s <scalar_value> <DimA_M> <DimA_N> <DimB_M> <DimB_N> <NumThreadsBloco> <MaxBlocoGrid> <QtdMaxMem> <matrixA_filename> <matrixB_filename> <result1_filename> <result2_filename>\n", argv[0]);
            return 0;
    }

    // Convert arguments
    scalar_value = strtof(argv[1], &eptr);
    matrixA.height = strtol(argv[2], &eptr, 10);
    matrixA.width = strtol(argv[3], &eptr, 10);
    matrixB.height = strtol(argv[4], &eptr, 10);
    matrixB.width = strtol(argv[5], &eptr, 10);
    threads_per_block = strtol(argv[6], &eptr, 10);
    max_blocks_per_grid = strtol(argv[7], &eptr, 10);
    max_mem_gpu = strtol(argv[8], &eptr, 10);
    max_mem_gpu = max_mem_gpu * 1024 * 1024;

    matrixC.height = matrixA.height;
    matrixC.width = matrixB.width;

    result1_filename = argv[11];
    result2_filename = argv[12];

    int limite = set_grid_size(threads_per_block,max_blocks_per_grid);

    if(limite == 0){
        printf("Erro ao setar o tamanho do grid\n");
        return 0;
    }
  
    /* Allocate the arrays of the four matrixes */

    matrixA.h_rows = (float*) malloc((matrixA.height * matrixA.width) * sizeof(float));
    matrixB.h_rows = (float*) malloc((matrixB.height * matrixB.width) * sizeof(float));
    matrixC.h_rows = (float*) malloc((matrixA.height * matrixB.width) * sizeof(float));

    /*Checks allocations*/
    if(matrixA.h_rows == NULL || matrixB.h_rows == NULL || matrixC.h_rows == NULL){
        printf("Erro ao alocar memoria\n");
        return 0;
    }

    //Alocando as matrizes na GPU
    cudaError = cudaMalloc(&matrixA.d_rows, (matrixA.height * matrixA.width) * sizeof(float));
    if (cudaError != cudaSuccess) {
        printf("cudaMalloc matrixA.d_rows returned error %s (code %d)\n", cudaGetErrorString(cudaError), cudaError);
        return 1;
    }

    cudaError = cudaMalloc(&matrixB.d_rows, (matrixB.height * matrixB.width) * sizeof(float));
    if (cudaError != cudaSuccess) {
        printf("cudaMalloc matrixB.d_rows returned error %s (code %d)\n", cudaGetErrorString(cudaError), cudaError);
        return 1;
    }

    cudaError = cudaMalloc(&matrixC.d_rows, (matrixC.height * matrixC.width) * sizeof(float));
    if (cudaError != cudaSuccess) {
        printf("cudaMalloc matrixC.d_rows returned error %s (code %d)\n", cudaGetErrorString(cudaError), cudaError);
        return 1;
    }

    /* Initialize the three matrixes */
    carregaA = load_matrix(&matrixA, argv[9]);
    carregaB = load_matrix(&matrixB, argv[10]);

    /*Checks if matrixes were loaded correctly */
    if(carregaA == 0 || carregaB == 0){
        printf("Erro ao carregar as matrizes\n");
        return 0;
    }

    inicializaC = initialize_matrix(&matrixC, 0.0f, 0.0f);

    if(inicializaC != 1){
        printf("Erro ao inicializar a matriz C\n");
        return 0;
    }

    somaTotalMemMatriz = matrixA.height * matrixA.width * sizeof(float) + matrixB.height * matrixB.width * sizeof(float) + matrixC.height * matrixC.width * sizeof(float);
    somaTotalMemB = matrixB.height * matrixB.width * sizeof(float);

    /*Se for viável fazer a alocação completa da três matrizes na CPU e na GPGPU, o programa deve
    atribuir o valor FULL_ALLOCATION no campo alloc_mode da três matrizes.
    */
    if(max_mem_gpu >= somaTotalMemMatriz){
        matrixA.alloc_mode = 1;
        matrixB.alloc_mode = 1;
        matrixC.alloc_mode = 1;
    }

    /* Se não for viável fazer a alocação simultânea e completa das matrizes A, B e C na GPGPU, o
    programa deve tentar alocar simultaneamente a matriz B por completo e o equivalente a uma das
    linhas da matriz A e uma das linhas da matriz C na GPGPU. Se tiver sucesso nessa alocação, o
    programa deve atribuir o valor FULL_ALLOCATION no campo alloc_mode da matriz B e o valor
    PARTIAL_ALLOC no campo alloc_mode das matrizes A e C.
    */
    else if(max_mem_gpu < somaTotalMemMatriz && max_mem_gpu >= somaTotalMemB){
        
        //Alocando a matriz B na GPU por completo
        cudaError = cudaMemcpy(matrixB.d_rows, matrixB.h_rows, (matrixB.height * matrixB.width) * sizeof(float), cudaMemcpyHostToDevice);
        if (cudaError != cudaSuccess) {
            printf("cudaMemcpy matrixB.h_rows -> matrixB.d_rows returned error %s (code %d)\n", cudaGetErrorString(cudaError), cudaError);
            return 1;
        }

        //alocando 1 linha de A na GPU
        for(int i = 0; i < matrixA.width; i++){
            cudaError = cudaMemcpy(matrixA.d_rows + i, matrixA.h_rows + i, sizeof(float), cudaMemcpyHostToDevice);
            if (cudaError != cudaSuccess) {
                printf("cudaMemcpy matrixA.h_rows -> matrixA.d_rows returned error %s (code %d)\n", cudaGetErrorString(cudaError), cudaError);
                return 1;
            }
        }

        //alocando 1 linha de C na GPU
        for(int j = 0;j < matrixC.width;j++){
            cudaError = cudaMemcpy(matrixC.d_rows + j, matrixC.h_rows + j, sizeof(float), cudaMemcpyHostToDevice);
            if (cudaError != cudaSuccess) {
                printf("cudaMemcpy matrixC.h_rows -> matrixC.d_rows returned error %s (code %d)\n", cudaGetErrorString(cudaError), cudaError);
                return 1;
            }
        }

        matrixA.alloc_mode = 0;
        matrixB.alloc_mode = 1;
        matrixC.alloc_mode = 0;
    }

    /*Se não for viável fazer a alocação completa da matriz B e a alocação parcial das matrizes A e C
    simultaneamente na GPGPU, o programa principal deve emitir uma notificação de erro de alocação
    de memória na GPGPU e encerrar sua execução.*/
    else{
        printf("Erro de alocação de memória na GPGPU\n");
        return 0;
    }

    //Alocando as matrizes na GPU por completo

    if(matrixA.alloc_mode == 1 && matrixB.alloc_mode == 1 && matrixC.alloc_mode == 1){
        cudaError = cudaMemcpy(matrixA.d_rows, matrixA.h_rows, (matrixA.height * matrixA.width) * sizeof(float), cudaMemcpyHostToDevice);
        if (cudaError != cudaSuccess) {
            printf("cudaMemcpy matrixA.h_rows -> matrixA.d_rows returned error %s (code %d)\n", cudaGetErrorString(cudaError), cudaError);
            return 1;
        }

        cudaError = cudaMemcpy(matrixB.d_rows, matrixB.h_rows, (matrixB.height * matrixB.width) * sizeof(float), cudaMemcpyHostToDevice);
        if (cudaError != cudaSuccess) {
            printf("cudaMemcpy matrixB.h_rows -> matrixB.d_rows returned error %s (code %d)\n", cudaGetErrorString(cudaError), cudaError);
            return 1;
        }

        cudaError = cudaMemcpy(matrixC.d_rows, matrixC.h_rows, (matrixC.height * matrixC.width) * sizeof(float), cudaMemcpyHostToDevice);
        if (cudaError != cudaSuccess) {
            printf("cudaMemcpy matrixC.h_rows -> matrixC.d_rows returned error %s (code %d)\n", cudaGetErrorString(cudaError), cudaError);
            return 1;
        }
    }

    /* Scalar product of matrix A */
    printf("Executing scalar_matrix_mult(%5.1f, matrixA)...\n",scalar_value);
    gettimeofday(&start, NULL);
    if (!scalar_matrix_mult(scalar_value, &matrixA)) {
	    printf("%s: scalar_matrix_mult problem.", argv[0]);
	    return 1;
    }
    gettimeofday(&stop, NULL);
    printf("%f ms\n", timedifference_msec(start, stop));

    cudaDeviceSynchronize();

    cudaError = cudaMemcpy(matrixA.h_rows, matrixA.d_rows, (matrixA.height * matrixA.width) * sizeof(float), cudaMemcpyDeviceToHost);
    if (cudaError != cudaSuccess){
	    printf("cudaMemcpy (d_y -> h_y) returned error %s (code %d), line(%d)\n", cudaGetErrorString(cudaError), cudaError, __LINE__);
	    return 1;
    }

    /* Print matrix */
    printf("---------- Matrix A ----------\n");
    print_matrix(&matrixA);

    /* Write first result */
    printf("Writing first result: %s...\n", result1_filename);
    if (!store_matrix(&matrixA, result1_filename)) {
        printf("%s: failed to write first result to file.", argv[0]);
        return 1;
    }

    /* Calculate the product between matrix A and matrix B */
    printf("Executing matrix_matrix_mult(matrixA, matrixB, matrixC)...\n");
    gettimeofday(&start, NULL);
    if (!matrix_matrix_mult(&matrixA, &matrixB, &matrixC)) {
	    printf("%s: matrix_matrix_mult problem.", argv[0]);
	    return 1;
    }
    gettimeofday(&stop, NULL);
    printf("%f ms\n", timedifference_msec(start, stop));

    cudaDeviceSynchronize();
    cudaError = cudaMemcpy(matrixC.h_rows, matrixC.d_rows, (matrixC.height * matrixC.width) * sizeof(float), cudaMemcpyDeviceToHost);
    if (cudaError != cudaSuccess){
        printf("cudaMemcpy (d_y -> h_y) returned error %s (code %d), line(%d)\n", cudaGetErrorString(cudaError), cudaError, __LINE__);
        return 1;
    }

    /* Print matrix */
    printf("---------- Matrix C ----------\n");
    print_matrix(&matrixC);

    /* Write second result */
    printf("Writing second result: %s...\n", result2_filename);
    if (!store_matrix(&matrixC, result2_filename)) {
        printf("%s: failed to write second result to file.", argv[0]);
        return 1;
    }

    /* Check foor errors */
    printf("Checking matrixC for errors...\n");
    gettimeofday(&start, NULL);

    /*Para checar com a matriz 1024 X 1024 basta mudar o float da check_errors para 51200.00f que é o valor esperado para multiplicar as matrizes com 10.0 e 5.0 */	
    if (check_errors(&matrixC, 51200.0f) == 1){
        printf("No errors found\n");
    };

    gettimeofday(&stop, NULL);
    printf("%f ms\n", timedifference_msec(start, stop));

    cudaFree(matrixA.d_rows);
    cudaFree(matrixB.d_rows);
    cudaFree(matrixC.d_rows);
    free(matrixA.h_rows);
    free(matrixB.h_rows);
    free(matrixC.h_rows);

    // Mark overall stop time
    gettimeofday(&overall_t2, NULL);

    // Show elapsed overall time
    printf("Overall time: %f ms\n", timedifference_msec(overall_t1, overall_t2));

    return 0;
}
