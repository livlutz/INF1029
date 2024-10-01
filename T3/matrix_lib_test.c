/*Lívia Lutz dos Santos - 2211055
Ana Luiza Pinto Marques - 2211960*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include "timer.h"
#include "matrix_lib.h"

float scalar_value = 0.0f;

struct matrix matrixA, matrixB, matrixC;

int store_matrix(struct matrix *matrix, char *filename) {
    FILE* arq = fopen(filename, "wb");
    int qtd = fwrite(matrix->rows, sizeof(float), matrix->height*matrix->width, arq);

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

    int qtd = fread(matrix->rows, sizeof(float), matrix->height*matrix->width, arq);

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
            matrix->rows[ind] = value;
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
            printf("%f ", matrix->rows[i * matrix->height + j]);
        }
        printf("\n");
    }

    return 1;
}

int check_errors(struct matrix *matrix, float scalar_value) {
    for(int i = 0; i < matrix->height; i++){
        for(int j = 0; j < matrix->width; j++){
            if(matrix->rows[i * matrix->height + j] != scalar_value){
                printf("Matrix error\nExpected value : %f\nReceived value:%f\n", scalar_value, matrix->rows[i * matrix->height + j]);
                return 0;
            }
        }
    }

    return 1;
}

/* Thread to initialize arrays */
void *init_arrays(void *threadarg) {
    struct thread_data *my_data;
    my_data = (struct thread_data*) threadarg;

    __m256 even,odd;
    even = _mm256_set1_ps(2.0f);
    odd = _mm256_set1_ps(5.0f);

  for(int i = my_data->offset_ini; i < my_data->offset_fim ; i+= 8){
    _mm256_store_ps(&my_data->a[i],even);
    _mm256_store_ps(&my_data->b[i],odd);
  }

}

int main(int argc, char *argv[]) {
    unsigned long int DimA_M, DimA_N, DimB_M, DimB_N;
    char *matrixA_filename, *matrixB_filename, *result1_filename, *result2_filename;
    char *eptr = NULL;
    struct timeval start, stop, overall_t1, overall_t2;
    int carregaA, carregaB, inicializaC,NumThreads,tam_arr;

    // Mark overall start time
    gettimeofday(&overall_t1, NULL);

    // Check arguments
    if (argc != 10) {
            printf("Usage: %s <scalar_value> <DimA_M> <DimA_N> <DimB_M> <DimB_N> <NumThreads> <matrixA_filename> <matrixB_filename> <result1_filename> <result2_filename>\n", argv[0]);
            return 0;
    }

    // Convert arguments
    scalar_value = strtof(argv[1], &eptr);
    matrixA.height = strtol(argv[2], &eptr, 10);
    matrixA.width = strtol(argv[3], &eptr, 10);
    matrixB.height = strtol(argv[4], &eptr, 10);
    matrixB.width = strtol(argv[5], &eptr, 10);
    set_number_threads(NumThreads);
    matrixC.height = matrixA.height;
    matrixC.width = matrixB.width;

    result1_filename = argv[9];
    result2_filename = argv[10];
  
    /* Allocate the arrays of the four matrixes */

    matrixA.rows = (float*) aligned_alloc(32, (matrixA.height * matrixA.width) * sizeof(float));
    matrixB.rows = (float*) aligned_alloc(32, (matrixB.height * matrixB.width) * sizeof(float));
    matrixC.rows = (float*) aligned_alloc(32, (matrixA.height * matrixB.width) * sizeof(float));

    /*Checks allocations*/
    if(matrixA.rows == NULL || matrixB.rows == NULL || matrixC.rows == NULL){
        printf("Erro ao alocar memoria\n");
        return 0;
    }

    /* Initialize the three matrixes */
    carregaA = load_matrix(&matrixA, argv[6]);
    carregaB = load_matrix(&matrixB, argv[7]);

    /*Checks if matrixes were loaded correctly */
    if(carregaA == 0 || carregaB == 0){
        printf("Erro ao carregar as matrizes\n");
        return 0;
    }

    inicializaC = initialize_matrix(&matrixC, 0.0f, 0.0f);
    tam_arr = matrixA.height * matrixA.width;

    pthread_t threads[NumThreads];
    struct thread_data thread_data_array[NumThreads];
    int slice = tam_arr/NumThreads;
    pthread_attr_t attr;
    void* status;
    int rc;

    /*Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    /* Create threads to initialize arrays */
    for(int i = 0; i < NumThreads;i++){
        printf("In main: creating thread %ld\n", i);
        thread_data_array[i].thread_id = i;
        thread_data_array[i].offset_ini =  thread_data_array[i].thread_id * slice;
        thread_data_array[i].offset_fim =  thread_data_array[i].offset_ini + slice; 
        thread_data_array[i].a = matrixA.rows;
        thread_data_array[i].b = matrixB.rows;
        thread_data_array[i].c = matrixC.rows;
        //nao sei qual a função q usa no lugar do init_arrays
        pthread_create(&threads[i],&attr,init_arrays,(void *)&thread_data_array[i]);
    }

    /*sincronização*/
    for(int t = 0; t < NumThreads;t++){
        rc = pthread_join(threads[t],&status);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    /* Scalar product of matrix A */
    printf("Executing scalar_matrix_mult(%5.1f, matrixA)...\n",scalar_value);
    gettimeofday(&start, NULL);
    /* Create threads to calculate product of arrays */
    for(int j = 0; j < NumThreads;j++){
        //tem q usar a scalar_matrix_mult no lugar de mult_arrays
        pthread_create(&threads[j],&attr,mult_arrays,(void *)&thread_data_array[j]);
    }

    /*sincronização*/
    for(int t = 0; t < NumThreads;t++){
        rc = pthread_join(threads[t],&status);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    if (!scalar_matrix_mult(scalar_value, &matrixA)) {
        printf("%s: scalar_matrix_mult problem.", argv[0]);
        return 1;
    }
    gettimeofday(&stop, NULL);
    printf("%f ms\n", timedifference_msec(start, stop));

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

    /* Create threads to calculate product of arrays */
    for(int j = 0; j < NumThreads;j++){
        //tem q usar matrix_matrix_mult no lugar de mult_arrays
        pthread_create(&threads[j],&attr,mult_arrays,(void *)&thread_data_array[j]);
    }

    for(int t = 0; t < NumThreads;t++){
        rc = pthread_join(threads[t],&status);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    if (!matrix_matrix_mult(&matrixA, &matrixB, &matrixC)) {
        printf("%s: matrix_matrix_mult problem.", argv[0]);
        return 1;
    }
    gettimeofday(&stop, NULL);
    printf("%f ms\n", timedifference_msec(start, stop));

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

    free(matrixA.rows);
    free(matrixB.rows);
    free(matrixC.rows);

    // Mark overall stop time
    gettimeofday(&overall_t2, NULL);

    // Show elapsed overall time
    printf("Overall time: %f ms\n", timedifference_msec(overall_t1, overall_t2));

    return 0;
}
