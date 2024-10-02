/*Lívia Lutz dos Santos - 2211055
Ana Luiza Pinto Marques - 2211960*/

#include "matrix_lib.h"
#include "timer.h"

static int NUMTHREADS;

/*Essa função recebe um valor escalar e uma matriz como argumentos de entrada e calcula o
produto do valor escalar pela matriz. O resultado da operação deve ser retornado na matriz
de entrada. Em caso de sucesso, a função deve retornar o valor 1. Em caso de erro, a
função deve retornar 0*/

int scalar_matrix_mult(float scalar_value, struct matrix *matrix){
    
    if(matrix == NULL){
        return 0;
    }

    //cria e da join das threads
    int tam_arr = matrix->height * matrix->width;
    pthread_t threads[NUMTHREADS];
    struct thread_data thread_data_array[NUMTHREADS];

    int slice = tam_arr/NUMTHREADS;
    pthread_attr_t attr;
    void* status;
    int rc;

    for(int i = 0; i< NUMTHREADS;i++){
        thread_data_array[i].thread_id = i;
        thread_data_array[i].offset_ini =  thread_data_array[i].thread_id * slice;
        thread_data_array[i].offset_fim =  thread_data_array[i].offset_ini + slice; 
        thread_data_array[i].a = matrix;
        thread_data_array[i].scalar = scalar_value;
        pthread_create(&threads[i],&attr,scalar_matrix_thread,(void *)&thread_data_array[i]);
    }
    
    for(int t = 0; t < NUMTHREADS;t++){
        rc = pthread_join(threads[t],&status);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    pthread_attr_destroy(&attr);

    return 1;

}

void* scalar_matrix_thread(void* threadarg){
    //faz a multiplicacao

    //pega a qtd de linhas / num de threads = N da conta

    struct thread_data *my_data;
    my_data = (struct thread_data*) threadarg;

    __m256 scalar = _mm256_set1_ps(my_data->scalar);

    for(int i = my_data->offset_ini;i < my_data->offset_fim;i += 8){
        __m256 row = _mm256_load_ps(&my_data->a->rows[i]);
        __m256 result = _mm256_mul_ps(row,scalar);
        _mm256_store_ps(&my_data->a->rows[i],result);
    }

}

/*Essa função recebe 3 matrizes como argumentos de entrada e calcula o valor do produto da
matriz A pela matriz B. O resultado da operação deve ser retornado na matriz C. Em caso
de sucesso, a função deve retornar o valor 1. Em caso de erro, a função deve retornar 0.*/

int matrix_matrix_mult(struct matrix *matrixA, struct matrix * matrixB, struct matrix * matrixC){

    //gerencia as threads (create e join) e chama a função da conta

    if((matrixA == NULL) || (matrixB == NULL) || (matrixC == NULL) || (matrixA->width != matrixB->height) || (matrixC->height != matrixA->height) || (matrixC->width != matrixB->width) ){
        printf("Erro de dimensao ou alocacao\n");
        return 0;
    }

    //cria e da join das threads
    int tam_arr = matrixC->height * matrixC->width;
    pthread_t threads[NUMTHREADS];
    struct thread_data thread_data_array[NUMTHREADS];

    int slice = tam_arr/NUMTHREADS;
    pthread_attr_t attr;
    void* status;
    int rc;

    for(int i = 0; i< NUMTHREADS;i++){
        thread_data_array[i].thread_id = i;
        thread_data_array[i].offset_ini =  thread_data_array[i].thread_id * slice;
        thread_data_array[i].offset_fim =  thread_data_array[i].offset_ini + slice; 
        thread_data_array[i].a = matrixA;
        thread_data_array[i].b = matrixB;
        thread_data_array[i].c = matrixC;
        pthread_create(&threads[i],&attr,matrix_matrix_mult_thread,(void *)&thread_data_array[i]);
    }
    
    for(int t = 0; t < NUMTHREADS;t++){
        rc = pthread_join(threads[t],&status);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    pthread_attr_destroy(&attr);

    return 1;
}

void* matrix_matrix_mult_thread(void* threadarg){
    //faz a multiplicacao

    //pegar a qtd de linhas / num de threads = N da conta

    struct thread_data *my_data;
    my_data = (struct thread_data*) threadarg;


    int indexA, indexB, indexC;

    for(int i = 0; i < my_data->c->height; i++){

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

    NUMTHREADS = num_threads;
}