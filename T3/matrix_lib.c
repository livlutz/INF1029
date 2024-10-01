/*Lívia Lutz dos Santos - 2211055
Ana Luiza Pinto Marques - 2211960*/

#include "matrix_lib.h"
#include "timer.h"

/*Essa função recebe um valor escalar e uma matriz como argumentos de entrada e calcula o
produto do valor escalar pela matriz. O resultado da operação deve ser retornado na matriz
de entrada. Em caso de sucesso, a função deve retornar o valor 1. Em caso de erro, a
função deve retornar 0*/

int scalar_matrix_mult(float scalar_value, struct matrix *matrix){
    
    //se o ponteiro da matrix for nulo (matrix nao existe), retorna erro
    if (matrix == NULL){
        return 0;
    }
    __m256 scalar = _mm256_set1_ps(scalar_value);

   // Usar instruções AVX para multiplicar todos os elementos da matriz por um escalar
   for(int i = 0; i< matrix->height * matrix->width; i += 8){
        __m256 row = _mm256_load_ps(&matrix->rows[i]);
        __m256 result = _mm256_mul_ps(row,scalar);
        _mm256_store_ps(&matrix->rows[i],result);
   }
       
    return 1;
}

void* scalar_matrix_thread(void* threadarg){
    struct thread_data *my_data;
    my_data = (struct thread_data*) threadarg;

    for(int i = my_data->offset_ini; i < my_data->offset_fim; i += 8){
        __m256 row = _mm256_load_ps(&my_data->a[i]);
        __m256 result = _mm256_mul_ps(row,my_data->scalar);
        _mm256_store_ps(&my_data->c[i],result);
    }
}

/*Essa função recebe 3 matrizes como argumentos de entrada e calcula o valor do produto da
matriz A pela matriz B. O resultado da operação deve ser retornado na matriz C. Em caso
de sucesso, a função deve retornar o valor 1. Em caso de erro, a função deve retornar 0.*/

int matrix_matrix_mult(struct matrix *matrixA, struct matrix * matrixB, struct matrix * matrixC){

    //gerencia as threads (create e join) e chama a função da conta

    /* Define auxiliary variables to work with threads */
    /*Ver como pegar o numero de threads e o tamanho do array*/
    pthread_t threads[NUM_THREADS];
    struct thread_data thread_data_array[NUM_THREADS];
    int slice = TAM_ARR/NUM_THREADS;
    pthread_attr_t attr;
    void* status;
    int rc;

    /*Criando as threads*/

    /*Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    /* Create threads to initialize arrays */
    for(int i = 0; i < NUM_THREADS;i++){
        printf("In main: creating thread %ld\n", i);
        thread_data_array[i].thread_id = i;
        thread_data_array[i].offset_ini =  thread_data_array[i].thread_id * slice;
        thread_data_array[i].offset_fim =  thread_data_array[i].offset_ini + slice; 
        thread_data_array[i].a = matrixA;
        thread_data_array[i].b = matrixB;
        thread_data_array[i].c = matrixC;
        pthread_create(&threads[i],&attr,matrix_matrix_mult_thread,(void *)&thread_data_array[i]);
    }

    /*sincronização*/
    for(int t = 0; t < NUM_THREADS;t++){
        rc = pthread_join(threads[t],&status);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
}

void* matrix_matrix_mult_thread(void* threadarg){
    //fazer a conta
    struct thread_data *my_data;
    my_data = (struct thread_data*) threadarg;

    //pegar a qtd de linhas / num de threads = N da conta
    int N = my_data->c->height/NUMTHREADS;

    /*se o ponteiro de alguma matrix for nulo (matrix nao existe), retorna erro
    o numero de colunas da primeira matrix tem q ser igual ao numero de linhas da segunda matrix
    a matriz resultante tem que ter o numero de linhas da primeira matriz e o numero de colunas da segunda matriz*/

    if((my_data->a == NULL) || (my_data->b == NULL) || (my_data->c == NULL) || (my_data->a->width != my_data->b->height) || (my_data->c->height != my_data->a->height) || (my_data->c->width != my_data->b->width) ){
        printf("Erro de dimensao ou alocacao\n");
        return 0;
    }

    int indexA, indexB, indexC;

    for(int i = my_data->offset_ini; i < my_data->offset_fim; i++){

        //itera por colunas da matriz A
        for(int j = 0; j < my_data->a->width;j++){
            //Calcula posicao inicial do indice da matrizA 
            indexA = i * my_data->a->width + j;

            //valor do elemento da matriz A
            __m256 valA = _mm256_set1_ps(my_data->a->rows[indexA]);

            //itera por colunas da matriz B
            for (int k = 0; k < my_data->b->width; k += 8){

                //Calcula posicao inicial do indice da matrizB
                indexB = j * my_data->b->width + k;
                //Calcula posicao inicial dos indices da matrizC aqui e depois incrementa o valor dentro do loop
                indexC = i * my_data->c->width + k;

                // Carrega o bloco de 8 elementos da linha j de B
                __m256 rowB = _mm256_load_ps(&my_data->b->rows[indexB]);

                // Carrega o bloco de 8 elementos da linha i de C 
                __m256 rowC = _mm256_load_ps(&my_data->c->rows[indexC]);

                //multiplica cada elemento da linha de A pelo elemento da coluna de B
                __m256 result = _mm256_fmadd_ps(rowB,valA,rowC);

                //Armazena o resultado na linha i de C
                _mm256_store_ps(&my_data->c->rows[indexC],result);
            }
        }
    }

}

void set_number_threads(int num_threads){

    num_threads = strtof(argv[6], &eptr);

    return;
}