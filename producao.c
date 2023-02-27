/*Crie um programa que simula uma linha de produção com 3 estágios.
No primeiro estágio 5 threads fundem peças brutas de alumínio, no
segundo estágio 5 threads pegam as peças brutas de alumínio e fazem
o polimento e, no terceiro estágio, 5 threads pegam as peças polidas e
fazem a pintura.

Mantenha 3 contadores: número de peças brutas fundidas esperando
polimento, número de peças polidas esperando pintura e número de
peças pintadas. 
Quando uma thread não tem peça para trabalhar ela precisa ficar
bloqueada (wait) até o estágio anterior gerar uma peça para ela.

Dica: o programa terá, além da thread main, 15 threads, 3 mutexes e 2
variáveis condição.
Coloque um sleep de 1s para etapa de trabalho.
O programa termina após a produção de 50 peças pintadas.*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#define nthread 5

pthread_t threads_fundir[nthread];
pthread_t threads_polimento[nthread];
pthread_t threads_pintura[nthread];

pthread_mutex_t mutex_fundir = PTHREAD_MUTEX_INITIALIZER;
int pecas_fundidas = 0;

pthread_mutex_t mutex_polimento[nthread];
int pecas_polidas = 0;

pthread_mutex_t mutex_fundir = PTHREAD_MUTEX_INITIALIZER;
int pecas_pintadas = 0;

pthread_cond_t tem_fundida = PTHREAD_COND_INITIALIZER;
pthread_cond_t tem_polida = PTHREAD_COND_INITIALIZER;

void fundir(void){
    while(1){
        sleep(1);
        pthread_mutex_lock(&pecas_fundidas);
        pecas_fundidas++;
        print("tem peca fundida\n");
        pthread_mutex_unlock(&pecas_fundidas);
    }
}

void polir(void){
    while(1){
        sleep(1);
        pthread_mutex_lock(&pecas_polidas);
        while(pecas_fundidas == 0)
            pthread_cond_signal(&tem_fundida, &mutex_fundir);
        pecas_polidas++;
        print("tem peca polida\n");
        print("tem peca polida");
        pthread_cond_signal(&tem_polida);

    }
}

int main(){
    

    return 0;
}