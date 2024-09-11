#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>

#define TAMARR 204800000

int main(int argc, char *argv[]){

  struct timeval start, stop;
  /* Aloca os tres arrays em memoria */ 

  float* aligned_evens = (float*) aligned_alloc(32,TAMARR * sizeof(float));
  float* aligned_odds = (float*) aligned_alloc(32,TAMARR * sizeof(float));
  float* aligned_c = (float*) aligned_alloc(32,TAMARR * sizeof(float));
  float* aligned_resultado = (float*) aligned_alloc(32,TAMARR * sizeof(float));

  printf("Initializing arrays...");

  gettimeofday(&start, NULL);
  
  /* Inicializa os dois arrays em memória */

  __m256 even,odd,C;
  even = _mm256_set1_ps(8.0f);
  odd = _mm256_set1_ps(5.0f);
  C = _mm256_set1_ps(3.0f);

  for(int i =0; i < TAMARR ; i+= 8){
    _mm256_store_ps(&aligned_evens[i],even);
    _mm256_store_ps(&aligned_odds[i],odd);
    _mm256_store_ps(&aligned_c[i],C);
  }
 
  for(int j = 0;j < TAMARR;j += 8){
    __m256 evens = _mm256_load_ps(&aligned_evens[j]); 
    __m256 odds = _mm256_load_ps(&aligned_odds[j]);
    __m256 cs = _mm256_load_ps(&aligned_c[j]);
    __m256 resultado = _mm256_fmadd_ps(evens,odds,cs);
    _mm256_store_ps(&aligned_resultado[j],resultado);
  }

  
  for (int k = 0; k < TAMARR; k++){
    if(aligned_resultado[k] != 43.0f){
      printf("Erro na multiplicacao\n");
      break;
    }
  }

  printf("Multiplicacao funciona :)\n");

  free(aligned_evens);
  free(aligned_odds);
  free(aligned_c);
  free(aligned_resultado);


  gettimeofday(&stop, NULL);
  printf("%f ms\n", timedifference_msec(start, stop));
  
  return 0;

}
