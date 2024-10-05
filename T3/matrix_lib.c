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

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


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

    for(int i = my_data->offset_ini; i < my_data->offset_fim; i += 8){
        __m256 row = _mm256_load_ps(&my_data->a->rows[i]);
        __m256 result = _mm256_mul_ps(row,scalar);
        _mm256_store_ps(&my_data->a->rows[i],result);
    }

    pthread_exit(NULL);
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

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


    for(int i = 0; i < NUMTHREADS; i++){
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

void* matrix_matrix_mult_thread(void* threadarg) {
    // Faz a multiplicação de matrizes
    struct thread_data *my_data;
    my_data = (struct thread_data*) threadarg;

    int linhas_por_thread = my_data->c->height / NUMTHREADS;
    int linha_inicio = my_data->thread_id * linhas_por_thread;
    int linha_fim = (my_data->thread_id == NUMTHREADS - 1) ? my_data->c->height : linha_inicio + linhas_por_thread;

    float *a_rows = my_data->a->rows;  // Ponteiro base da matriz A
    float *b_rows = my_data->b->rows;  // Ponteiro base da matriz B
    float *c_rows = my_data->c->rows;  // Ponteiro base da matriz C

    float* a_row, *b_row, *c_row;

    __m256 valA, rowB, rowC, result;

    int a_width = my_data->a->width;   // Largura da matriz A (número de colunas)
    int b_width = my_data->b->width;   // Largura da matriz B (número de colunas)
    int c_width = my_data->c->width;   // Largura da matriz C (número de colunas)

    int index_a, pre_index_b, index_b, index_c;

    for (int i = linha_inicio; i < linha_fim; i++) {  // i itera sobre as linhas da matriz C
        index_c = i * c_width;
        index_a = i * a_width;
        c_row = &c_rows[index_c];          // Ponteiro para a linha i da matriz C

        for (int j = 0; j < a_width; j++) {           // Itera sobre as colunas da matriz A
            pre_index_b = j* b_width;
            a_row = &a_rows[index_a];      // Ponteiro para a linha i da matriz A
            valA = _mm256_set1_ps(a_row[j]);   // Carrega o valor de A para multiplicar com a linha de B

            for (int k = 0; k < b_width; k += 8) {    // Itera sobre as colunas da matriz B e processa 8 elementos por vez
                index_b = pre_index_b + k;
                if (k + 8 > b_width) {
                    break; // Evita ultrapassar o limite
                }

                b_row = &b_rows[index_b];  // Ponteiro para a linha j da matriz B (8 elementos)
                rowB = _mm256_load_ps(b_row);      // Carrega 8 elementos da linha de B
                rowC = _mm256_load_ps(&c_row[k]);  // Carrega 8 elementos da linha i de C

                // Multiplica cada elemento da linha de A pelo elemento correspondente da coluna de B e acumula em C
                result = _mm256_fmadd_ps(rowB, valA, rowC);

                // Armazena o resultado na linha i de C
                _mm256_store_ps(&c_row[k], result);
            }
        }
    }

    pthread_exit(NULL);
}

void set_number_threads(int num_threads){

    NUMTHREADS = num_threads;
}