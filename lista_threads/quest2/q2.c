#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 5
#define NUM_NODES 5

//grafo de alocacao de recursos
int grafo[NUM_NODES][NUM_NODES] = {
    {0, 1, 0, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 0, 1, 0},
    {0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0}
};

//mutex para controlar acesso a variavel deadlock
pthread_mutex_t mutex_deadlock;
int deadlock = 0;

//funcao de busca em profundidade
int busca_em_profundidade(int inicio, int *visitados) {
    visitados[inicio] = 1;
    for (int j = 0; j < NUM_NODES; j++) {
        if (grafo[inicio][j]) {
            if (visitados[j] || busca_em_profundidade(j, visitados)) {
                return 1;
            }
        }
    }
    visitados[inicio] = 0;
    return 0;
}

//funcao executada pelas threads
void *verificar_deadlock(void *arg) {
    int inicio = *((int *)arg);
    int visitados[NUM_NODES] = {0};

    if (busca_em_profundidade(inicio, visitados)) {
        pthread_mutex_lock(&mutex_deadlock);
        deadlock = 1;
        pthread_mutex_unlock(&mutex_deadlock);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];

    //inicializa o mutex
    pthread_mutex_init(&mutex_deadlock, NULL);

    //cria e inicia as threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, verificar_deadlock, (void *)&thread_args[i]);
    }

    //aguarda todas as threads terminarem
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    //verifica se um deadlock foi detectado
    if (deadlock) {
        printf("deadlock detectado!\n");
    } else {
        printf("nenhum deadlock detectado.\n");
    }

    //destroi o mutex e encerra o programa
    pthread_mutex_destroy(&mutex_deadlock);

    return 0;
}
