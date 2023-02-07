#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 2     
#define LINHAS 3
#define COLUNAS 3


int matriz1[3][3] = { {1, 2,5}, {3, 4,2}, {5, 6,1} };
int matriz2[3][3] = {{1, 2,5}, {3, 4,2},  {5, 6,1} };
int resultado[3][3];
  

void *threadCode(void *tid){   
	
	int i,j, k;
  int  threadId = (*(int *)tid); 
	
	   for(i=threadId; i < LINHAS; i = i + NUM_THREADS) {
	   	      	
      			for (j=0;j<COLUNAS;j++) {
		        	 resultado[i][j] = 0;
  
       				 for(k=0;k< COLUNAS;k++) {
           		 		resultado[i][j] = resultado[i][j] + matriz1[i][k]* matriz2[k][j];
       		 		}
      			}			
       
    	}
  
}

int main (int argc, char *argv[]){   
	pthread_t threads[NUM_THREADS]; 
  int *taskids[NUM_THREADS];
	int i,j,u; int t;   
	
  for(t=0; t<NUM_THREADS; t++){
    printf("No main: criando thread %d\n", t);    
    taskids[t] = (int *) malloc(sizeof(int)); *taskids[t] = t;
	 	pthread_create(&threads[t],NULL,threadCode, (void *)taskids[t]);         
  }
  
  for(u=0; u<NUM_THREADS;u++) {
    int *res;
    pthread_join(threads[u], NULL);
  }   
  
  for(i=0; i < LINHAS; i++) {
			 for (j=0;j<COLUNAS;j++) {
      		 printf("%d\t", resultado[i][j]);
  		 }
  		 
  		 printf("\n");
   }
        		
  
  pthread_exit(NULL);
}