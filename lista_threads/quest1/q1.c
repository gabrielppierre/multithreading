#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 10
#define TARGET_VALUE 1000000

//variavel global que sera compartilhada pelas threads
int counter = 0;

//variaveis de condicao e mutex para controle de acesso e sinalizacao de parada
pthread_mutex_t mutex;
pthread_cond_t stop_cond;

//funcao que sera executada pelas threads
void *increment_counter(void *thread_id) {
    size_t tid = (size_t)thread_id;
    while (1) {
        //lock no mutex
        pthread_mutex_lock(&mutex);

        //verifica se o contador atingiu o valor desejado
        if (counter >= TARGET_VALUE) {
            pthread_cond_broadcast(&stop_cond);
            pthread_mutex_unlock(&mutex);
            break;
        }

        //incrementa o contador global
        counter++;

        //verifica novamente se o contador atingiu o valor desejado
        if (counter >= TARGET_VALUE) {
            printf("Thread %zu atingiu o valor %d\n", tid, TARGET_VALUE);
            pthread_cond_broadcast(&stop_cond);
        }

        //unlock no mutex
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int rc;

    //inicializa o mutex e a variavel de condicao
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&stop_cond, NULL);

    //cria as threads
    for (size_t t = 0; t < NUM_THREADS; t++) {
        printf("criando a thread %zu\n", t);
        rc = pthread_create(&threads[t], NULL, increment_counter, (void *)t);
        if (rc) {
            printf("erro: codigo de retorno de pthread_create() eh %d\n", rc);
            exit(-1);
        }
    }

    //aguarda as threads finalizarem
    for (size_t t = 0; t < NUM_THREADS; t++) {
        rc = pthread_join(threads[t], NULL);
        if (rc) {
            printf("erro: codigo de retorno de pthread_join() eh %d\n", rc);
            exit(-1);
        }
    }

    //destroi o mutex e a variavel de condicao
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&stop_cond);

    printf("valor final do contador global: %d\n", counter);
    pthread_exit(NULL);
}
