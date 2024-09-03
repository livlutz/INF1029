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
    for(int i = 0; i < matrix->height; i++){
        for(int j = 0; j < matrix->width; j++){
            matrix->rows[i * matrix->height + j] = value;
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
		    break;
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
                printf("Matrix error\n");
                return 0;
            }
        }
    }

    return 1;
}

int main(int argc, char *argv[]) {
  unsigned long int DimA_M, DimA_N, DimB_M, DimB_N;
  char *matrixA_filename, *matrixB_filename, *result1_filename, *result2_filename;
  char *eptr = NULL;
  struct timeval start, stop, overall_t1, overall_t2;
  int carregaA, carregaB, inicializaC;

  // Mark overall start time
  gettimeofday(&overall_t1, NULL);

  // Check arguments
  if (argc != 10) {
        printf("Usage: %s <scalar_value> <DimA_M> <DimA_N> <DimB_M> <DimB_N> <matrixA_filename> <matrixB_filename> <result1_filename> <result2_filename>\n", argv[0]);
        return 0;
  }

  // Convert arguments

  scalar_value = strtof(argv[1], &eptr);
  matrixA.height = strtol(argv[2], &eptr, 10);
  matrixA.width = strtol(argv[3], &eptr, 10);
  matrixB.height = strtol(argv[4], &eptr, 10);
  matrixB.width = strtol(argv[5], &eptr, 10);
  matrixC.height = matrixA.height;
  matrixC.width = matrixB.width;
  

  result1_filename = argv[8];
  result2_filename = argv[9];
  
  /* Allocate the arrays of the four matrixes */

  matrixA.rows = (float*) malloc( (matrixA.height * matrixA.width) * sizeof(float));
  matrixB.rows = (float*) malloc( (matrixB.height * matrixB.width) * sizeof(float));
  matrixC.rows = (float*) malloc( (matrixA.height * matrixB.width) * sizeof(float));

  //verifica se a matriz foi alocada corretamente
  if(matrixA.rows == NULL || matrixB.rows == NULL || matrixC.rows == NULL){
      printf("Erro ao alocar memoria\n");
      return 0;
  }

  /* Initialize the three matrixes */
  carregaA = load_matrix(&matrixA, argv[6]);
  carregaB = load_matrix(&matrixB, argv[7]);

  if(carregaA == 0 || carregaB == 0){
      printf("Erro ao carregar as matrizes\n");
      return 0;
  }

  inicializaC = initialize_matrix(&matrixC, 0.0f, 0.0f);

  /* Scalar product of matrix A */
  printf("Executing scalar_matrix_mult(%5.1f, matrixA)...\n",scalar_value);
  gettimeofday(&start, NULL);
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
  printf("Executing matrix_matrix_mult(matrixA, mattrixB, matrixC)...\n");
  gettimeofday(&start, NULL);
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
  check_errors(&matrixC, 800.0f);
  gettimeofday(&stop, NULL);
  printf("%f ms\n", timedifference_msec(start, stop));

  free(matrixA.rows);
  free(matrixB.rows);
  free(matrixC.rows);

  // Mark overall stop time
  gettimeofday(&overall_t2, NULL);

  // Show elapsed overall time
  printf("Overall time: %f ms\n", timedifference_msec(overall_t1, overall_t2));

  // Show processor used!! - type lscpu on the terminal

  return 0;
}
