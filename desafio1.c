/* 
Crie um programa com 5 threads usando laço e 
passe para cada uma o nome da respectiva thread.
Cada thread dorme por 10 segundos de 1 em 1 segundo.
Cada thread retorna ao termino um codigo de ERRO caso o
seu nome nao inicie com letra maiuscula, e SUCESSO caso inicie. 
*/

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define tam_max 10
#define nthreads 5

typedef struct{
    char* nome;
}Parametro;

void* tarefa(Parametro* thread){
    for(int i=0; i<10; i++){
        sleep(1);
        printf("passaram-se %ds em %s de um total de 10s\n", i, thread->nome);
    }
    if(thread->nome[0] >= 'A' && thread->nome[0] <= 'Z')
        printf("SUCESSO %s\n", thread->nome);
    else
        printf("ERRO %s\n", thread->nome);
}

pthread_t threads[nthreads];
Parametro parametros[nthreads];

char Nome[nthreads][tam_max];

int main(){
    for(int i=0; i<nthreads; i++){
        printf("\nnome da %d thread: ", i+1);
        scanf("%s", Nome[i]);
        parametros[i].nome = Nome[i];
        int ret = pthread_create(&threads[i], NULL, (void*) tarefa, (void*) &parametros[i]);
        if(ret != 0) {
            fprintf(stderr, "Erro ao criar thread\n");
            exit(1);
        }
    }

    for(int i=0; i<nthreads; i++){
        int ret = pthread_join(threads[i], NULL);
        if(ret != 0) {
            fprintf(stderr, "Erro no joining da thread %s\n", parametros[i].nome);
            exit(1);
        }
    }

    return 0;
}
