#ifndef API_H_
#define API_H_

#include <pthread.h>

#define MAX_EXEC 100 // tamanho máximo da fila de execuções
#define MAX_RESULT 100 // tamanho máximo da área temporária de armazenamento dos resultados

typedef struct {
    void *(*funexec)(void *); // função a ser executada
    void *param; // parâmetros da função
    int id; // identificador da execução
} Exec;

typedef struct {
    Exec execs[MAX_EXEC]; // buffer de execuções pendentes
    int start; // índice de início da fila
    int end; // índice de fim da fila
    int count; // número de execuções pendentes
    pthread_mutex_t mutex; // mutex para controle de acesso ao buffer
    pthread_cond_t cond; // variável de condição para notificar o despachante sobre novas execuções
} Buffer;

typedef struct {
    void *result; // resultado da execução da função
    int id; // identificador da execução
    int ready; // flag que indica se o resultado está pronto
    pthread_mutex_t mutex; // mutex para controle de acesso à área de resultados
} Result;

typedef struct {
    int id; // identificador da thread
    pthread_t thread; // thread correspondente
    int busy; // flag que indica se a thread está ocupada
} Worker;

typedef struct {
    int n; // número de processadores ou núcleos do sistema
    Buffer buffer; // buffer de execuções pendentes
    Result results[MAX_RESULT]; // área temporária de armazenamento dos resultados
    Worker workers[100]; // conjunto de threads trabalhadoras
    pthread_mutex_t mutex; // mutex para controle de acesso às threads trabalhadoras
    pthread_cond_t cond; // variável de condição para notificar o despachante sobre threads livres
} API;

API *criarAPI(int n);
void destruirAPI(API *api);
int agendarExecucao(API *api, void *(*funexec)(void *), void *param);
void *pegarResultadoExecucao(API *api, int id);

#endif /* API_H_ */
