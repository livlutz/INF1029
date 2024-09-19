#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <immintrin.h>
#include "timer.h"

#define NUM_THREADS 4
#define TAM_ARR 204800000

struct thread_data {
   long thread_id;
   float* arr_evens;
   float* arr_odds;
   float* arr_result;
   int  offset_ini;
   int  offset_fim;
   char *message;
};

/* Thread to multiply arrays */
void *mult_arrays(void *threadarg) {
    struct thread_data *my_data;
    my_data = (struct thread_data*) threadarg;

    for(int i = my_data->offset_ini;i < my_data->offset_fim;i += 8){
        __m256 evens = _mm256_load_ps(&my_data->arr_evens[i]); 
        __m256 odds = _mm256_load_ps(&my_data->arr_odds[i]);
        __m256 resultado = _mm256_mul_ps(evens,odds);
        _mm256_store_ps(&my_data->arr_result[i],resultado);
    }
}

/* Thread to initialize arrays */
void *init_arrays(void *threadarg) {
    struct thread_data *my_data;
    my_data = (struct thread_data*) threadarg;

    __m256 even,odd;
    even = _mm256_set1_ps(2.0f);
    odd = _mm256_set1_ps(5.0f);

  for(int i = my_data->offset_ini; i < my_data->offset_fim ; i+= 8){
    _mm256_store_ps(&my_data->arr_evens[i],even);
    _mm256_store_ps(&my_data->arr_odds[i],odd);
  }

}


int main(int argc, char *argv[]) {

    /* Allocate three arrays */
    float*evens,*odds,*result;
    evens = (float*)aligned_alloc(32,TAM_ARR*sizeof(float));
    odds = (float*)aligned_alloc(32,TAM_ARR*sizeof(float));
    result = (float*)aligned_alloc(32,TAM_ARR*sizeof(float));

    /* Define auxiliary variables to work with threads */
    pthread_t threads[NUM_THREADS];
    struct thread_data thread_data_array[NUM_THREADS];
    int slice = TAM_ARR/NUM_THREADS;
    pthread_attr_t attr;
    void* status;
    int rc;
    struct timeval s, stop, overall_t1, overall_t2;

    // Mark overall start time
    gettimeofday(&overall_t1, NULL);

    /*Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    gettimeofday(&s, NULL);
    /* Create threads to initialize arrays */
    for(int i = 0; i < NUM_THREADS;i++){
        printf("In main: creating thread %ld\n", i);
        thread_data_array[i].thread_id = i;
        thread_data_array[i].offset_ini =  thread_data_array[i].thread_id * slice;
        thread_data_array[i].offset_fim =  thread_data_array[i].offset_ini + slice; 
        thread_data_array[i].arr_evens = evens;
        thread_data_array[i].arr_odds = odds;
        thread_data_array[i].arr_result = result;
        pthread_create(&threads[i],&attr,init_arrays,(void *)&thread_data_array[i]);
    }

    /*sincronização*/
    for(int t = 0; t < NUM_THREADS;t++){
        rc = pthread_join(threads[t],&status);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    gettimeofday(&stop, NULL);
    printf("Inicializacao : %f ms\n", timedifference_msec(s, stop));
    
    gettimeofday(&s, NULL);
    /* Create threads to calculate product of arrays */
    for(int j = 0; j < NUM_THREADS;j++){
        printf("In main: creating thread multiply %ld\n", j);
        pthread_create(&threads[j],&attr,mult_arrays,(void *)&thread_data_array[j]);
    }

    /*sincronização*/
    for(int t = 0; t < NUM_THREADS;t++){
        rc = pthread_join(threads[t],&status);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    gettimeofday(&stop, NULL);
    printf("Multiplicacao : %f ms\n", timedifference_msec(s, stop));

    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);

    printf("Iniciando check\n");

    /* Check for errors (all values should be 10.0f) */
    for(int i = 0;i < TAM_ARR;i++){
        if(result[i] != 10.0f){
            printf("Erro na multiplicação\nValor esperado : 10.0\n Valor recebido :%.1f\n",result[i]);
            break;
        }
    }

    /* Free memory */
    free(evens);
    free(odds);
    free(result);

    gettimeofday(&overall_t2, NULL);

    printf("Overall time: %f ms\n", timedifference_msec(overall_t1, overall_t2));
    return 0;
}

/*gcc –std=c11 –pthread –mavx –o ex1_comavx ex1_comavx.c timer.c*/