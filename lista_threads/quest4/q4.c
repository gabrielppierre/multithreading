#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_N 4
#define P 6

int num_threads = 4;


pthread_t *threads;
pthread_barrier_t barrier;

int k = 0;

// esse exemplo foi pré definido pelos participantes 

int A[NUM_N][NUM_N] = {
    {13, 2, 1, 8 },
    {1, 5, 10, 17},
    {6, 22, 4, 13},
    {27, 7, 3, 18},  
};

int B[NUM_N] = {12, 5, 6, 9};

double X[NUM_N][P+1] = {
   {1, 0, 0, 0, 0, 0},
   {1, 0, 0, 0, 0, 0},
   {1, 0, 0, 0, 0, 0},
   {1, 0, 0, 0, 0, 0},
};

// estrutura para cada thread
typedef struct {
    int start;
    int end;
} range_t;

void* calcX(void* arg) {
    range_t range = *((range_t*)arg);
    int start = range.start;
    int end = range.end;
    double sum = 0;

 // aqui eh executado o calculo solicitado (por iteração). Estaremos adorando 6 iterações
    while (k < P) {
        
        for (int i = start; i < end; i++) {
            for (int j = 0; j < NUM_N; j++) {
                if (i != j) sum += A[i][j] * X[j][k];
            }
            X[i][k+1] = (1.0 / A[i][i]) * (B[i] - sum);
            printf("X==%f\n", X[i][k+1]);
            sum = 0;
           pthread_barrier_wait(&barrier);
        }
        pthread_barrier_wait(&barrier);
        k++;
      if(k==5) break;
    }

    pthread_exit(NULL);
}

int main() {

// Na questão foi solicitado rodar o código em uma máquina de vários núcleos. Ambos os integrantes só tem 1 núcleo
// O código testado só executa até 2 threads em paralelo

    printf("Digite o número de threads: ");
    scanf("%d", &num_threads);

    threads = malloc(num_threads* sizeof(pthread_t));

    pthread_barrier_init(&barrier, NULL, num_threads);

    int chunk_size = ceil((float)NUM_N / num_threads);
    printf(" chunk_size: %d\n", chunk_size);
    for (int i = 0; i < num_threads; i++) {
        range_t* arg = malloc(sizeof(range_t));
   // essa estrutura limita  a quantidade de icognitas por iteração que a thread vai executar
        arg->start = i * chunk_size;
        arg->end = (i + 1) * chunk_size < NUM_N ? (i + 1) * chunk_size : NUM_N;
        pthread_create(&threads[i], NULL, calcX, arg);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

// aqui estamos printando os valores das icognitas calculadas por iteração. Cada coluna 1 iteração

    printf("Resultado:\n");
    for (int i = 0; i < NUM_N; i++) {
        for (int j = 0; j < P ; j++) {
            printf("%.2f ", X[i][j]);
        }
        printf("\n");
    }

    pthread_barrier_destroy(&barrier);
    free(threads);
    return 0;
}

