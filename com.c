#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

//declarando threads
pthread_t t1;
pthread_t t2;

//struct para armazenar parametros da thread
typedef struct{
    char *name;
    char cont;
}dados;


void tarefa1(dados *dado){
    for(int i=0; i< dado->cont; i++){
        sleep(1);
        printf("passaram-se %d segundos na thread %s\n", i+1, dado->name);
    }
    
}


int main(){
    dados parametros;
    parametros.name = "thread um";
    parametros.cont = 10;

    pthread_create(&t1, NULL, (void*) tarefa1, (void*) &parametros);//precisa passar um endereco nos parametros
    pthread_join(t1, NULL);

    return 0;
}