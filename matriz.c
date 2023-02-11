//programa em C que multiplica linhas e colunas de uma matriz usando multithreading

#include <pthread.h>
#include <stdio.h>

#define NUM_THREADS 8
#define M 4
#define N 5
#define K 6

int A[M][K];
int B[K][N];
int C[M][N];

struct v {
   int i; /* linha */
   int j; /* coluna */
};

void *multiplicação(void *param) {
   struct v *dados = (struct v *)param;
   int i = dados->i;
   int j = dados->j;
   int k;
   int soma = 0;
   for(k=0; k<K; k++) {
      soma += A[i][k] * B[k][j];
   }
   C[i][j] = soma;
   pthread_exit(0);
}

int main() {
   int i, j;
   pthread_t threads[NUM_THREADS];
   int contador = 0;
   for(i=0; i<M; i++) {
      for(j=0; j<N; j++) {
         struct v *dados = (struct v *) malloc(sizeof(struct v));
         dados->i = i;
         dados->j = j;
         pthread_create(&threads[contador], NULL, multiplicação, (void *)dados);
         contador++;
      }
   }

   for(i=0; i<NUM_THREADS; i++) {
      pthread_join(threads[i], NULL);
   }

   printf("Resultado da matriz:\n");
   for(i=0; i<M; i++) {
      for(j=0; j<N; j++) {
         printf("%d ", C[i][j]);
      }
      printf("\n");
   }
   pthread_exit(0);
}
