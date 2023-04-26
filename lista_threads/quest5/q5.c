#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//estrutura de um elemento da fila
typedef struct elem {
   int value;            //valor do elemento
   struct elem *prox;    //ponteiro para o proximo elemento da fila
} Elem;

//estrutura da fila bloqueante
typedef struct blockingQueue {
   unsigned int sizeBuffer, statusBuffer; //tamanho maximo e tamanho atual da fila
   Elem *head, *last;                     //ponteiros para o primeiro e último elementos da fila
   pthread_mutex_t mutex;                 //mutex para garantir a exclusao mútua
   pthread_cond_t notFull;                //variavel condicional para controlar quando a fila nao esta cheia
   pthread_cond_t notEmpty;               //variavel condicional para controlar quando a fila nao esta vazia
} BlockingQueue;

//funcao para criar uma nova fila bloqueante com o tamanho especificado
BlockingQueue* newBlockingQueue(unsigned int SizeBuffer) {
    BlockingQueue* queue = (BlockingQueue*) malloc(sizeof(BlockingQueue));
    queue->sizeBuffer = SizeBuffer;
    queue->statusBuffer = 0;
    queue->head = NULL;
    queue->last = NULL;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->notFull, NULL);
    pthread_cond_init(&queue->notEmpty, NULL);
    return queue;
}

//funcao para inserir um valor na fila bloqueante
void putBlockingQueue(BlockingQueue* Q, int newValue) {
    pthread_mutex_lock(&Q->mutex);
    while (Q->statusBuffer == Q->sizeBuffer) {
        printf("A fila esta cheia! Thread produtora dormindo...\n");
        pthread_cond_wait(&Q->notFull, &Q->mutex);
    }
    Elem* newElem = (Elem*) malloc(sizeof(Elem));
    newElem->value = newValue;
    newElem->prox = NULL;
    if (Q->statusBuffer == 0) {
        Q->head = newElem;
    } else {
        Q->last->prox = newElem;
    }
    Q->last = newElem;
    Q->statusBuffer++;
    printf("Inserido valor %d na fila\n", newValue);
    pthread_cond_signal(&Q->notEmpty);
    pthread_mutex_unlock(&Q->mutex);
}

//funcao para retirar um valor da fila bloqueante
int takeBlockingQueue(BlockingQueue* Q) {
    pthread_mutex_lock(&Q->mutex);
    while (Q->statusBuffer == 0) {
        printf("A fila esta vazia! Thread consumidora dormindo...\n");
        pthread_cond_wait(&Q->notEmpty, &Q->mutex);
    }
    int value = Q->head->value;
    Elem* elemToRemove = Q->head;
    Q->head = Q->head->prox;
    free(elemToRemove);
    Q->statusBuffer--;
    printf("Retirado valor %d da fila\n", value);
    pthread_cond_signal(&Q->notFull);
    pthread_mutex_unlock(&Q->mutex);
    return value;
}

//funcao executada pelas threads produtoras
void* producer(void* arg) {
    BlockingQueue* Q = (BlockingQueue*) arg;
    while (1) {
        int newValue = rand() % 10;
        putBlockingQueue(Q, newValue);
        sleep(1);
    }
}

//funcao executada pelas threads consumidoras
void* consumer(void* arg) {
    BlockingQueue* Q = (BlockingQueue*) arg;
    while (1) {
        int value = takeBlockingQueue(Q);
        sleep(1);
    }
}

int main() {
    BlockingQueue* Q = newBlockingQueue(5); //cria uma fila bloqueante de tamanho 5
    pthread_t producers[2], consumers[2];  //cria vetores para armazenar os IDs das threads produtoras e consumidoras
    int i;
    //cria duas threads produtoras
    for (i = 0; i < 2; i++) {
        pthread_create(&producers[i], NULL, producer, (void*) Q);
    }
    //cria duas threads consumidoras
    for (i = 0; i < 2; i++) {
        pthread_create(&consumers[i], NULL, consumer, (void*) Q);
    }
    //aguarda as threads produtoras finalizarem
    for (i = 0; i < 2; i++) {
        pthread_join(producers[i], NULL);
    }
    //Aguarda as threads consumidoras finalizarem
    for (i = 0; i < 2; i++) {
        pthread_join(consumers[i], NULL);
    }
    //libera os recursos das variaveis condicionais e do mutex
    pthread_mutex_destroy(&Q->mutex);
    pthread_cond_destroy(&Q->notFull);
    pthread_cond_destroy(&Q->notEmpty);
    //libera a memória alocada para a fila bloqueante
    free(Q);
    return 0;
}
