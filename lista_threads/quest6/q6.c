#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_EXEC 100 //tamanho maximo da fila de execucoes
#define MAX_RESULT 100 // Tamanho maximo da área temporaria de armazenamento dos resultados

typedef struct {
  void * ( * funexec)(void * ); //funcao a ser executada
  void * param; //parametros da funcao
  int id; //identificador da execucao
}
Exec;

typedef struct {
  Exec execs[MAX_EXEC]; //buffer de execucoes pendentes
  int start; //indice de inicio da fila
  int end; //indice de fim da fila
  int count; //número de execucoes pendentes
  pthread_mutex_t mutex; //mutex para controle de acesso ao buffer
  pthread_cond_t cond; //variavel de condição para notificar o despachante sobre novas execucoes
}
Buffer;

typedef struct {
  void * result; //resultado da execucao da funao
  int id; //identificador da execuao
  int ready; //flag que indica se o resultado esta pronto
  pthread_mutex_t mutex; //mutex para controle de acesso a area de resultados
}
Result;

typedef struct {
  int id; //identificador da thread
  pthread_t thread; //thread correspondente
  int busy; //flag que indica se a thread esta ocupada
}
Worker;

typedef struct {
  int n; //numero de processadores ou nucleos do sistema
  Buffer buffer; //buffer de execucoes pendentes
  Result results[MAX_RESULT]; //area temporaria de armazenamento dos resultados
  Worker workers[100]; //conjunto de threads trabalhadoras
  pthread_mutex_t mutex; //mutex para controle de acesso as threads trabalhadoras
  pthread_cond_t cond; //variavel de condicao para notificar o despachante sobre threads livres
}
API;

int next_id = 1; //variavel global para controle dos identificadores das execucoes

void * worker_thread(void * arg) {
  API * api = (API * ) arg;

  while (1) {
    pthread_mutex_lock( & api -> mutex);

    while (api -> buffer.count == 0 || api -> workers[api -> n - 1].busy) {
      pthread_cond_wait( & api -> cond, & api -> mutex);
    }

    int i;

    for (i = 0; i < api -> n; i++) {
      if (!api -> workers[i].busy) {
        api -> workers[i].busy = 1;
        break;
      }
    }

    Exec exec = api -> buffer.execs[api -> buffer.start];
    api -> buffer.start = (api -> buffer.start + 1) % MAX_EXEC;
    api -> buffer.count--;

    pthread_mutex_unlock( & api -> mutex);

    void * result = exec.funexec(exec.param);

    pthread_mutex_lock( & api -> mutex);

    int j;

    for (j = 0; j < MAX_RESULT; j++) {
      if (api -> results[j].id == exec.id) {
        api -> results[j].result = result;
        api -> results[j].ready = 1;
        break;
      }
    }

    for (i = 0; i < api -> n; i++) {
      if (api -> workers[i].id == pthread_self()) {
        api -> workers[i].busy = 0;
        break;
      }
    }

    pthread_cond_signal( & api -> cond);
    pthread_mutex_unlock( & api -> mutex);
  }

  return NULL;
}

int agendarExecucao(API * api, void * ( * funexec)(void * ), void * param) {
  pthread_mutex_lock( & api -> buffer.mutex);

  if (api -> buffer.count == MAX_EXEC) {
    pthread_mutex_unlock( & api -> buffer.mutex);
    return -1;
  }

  int id = next_id++;

  api -> buffer.execs[api -> buffer.end].funexec = funexec;
  api -> buffer.execs[api -> buffer.end].param = param;
  api -> buffer.execs[api -> buffer.end].id = id;
  api -> buffer.end = (api -> buffer.end + 1) % MAX_EXEC;
  api -> buffer.count++;

  pthread_cond_signal( & api -> buffer.cond);
  pthread_mutex_unlock( & api -> buffer.mutex);

  return id;
}

void * pegarResultadoExecucao(API * api, int id) {
  pthread_mutex_lock( & api -> mutex);

  int i, j;

  for (i = 0; i < MAX_RESULT; i++) {
    if (api -> results[i].id == id) {
      while (!api -> results[i].ready) {
        pthread_cond_wait( & api -> cond, & api -> mutex);
      }

      void * result = api -> results[i].result;

      api -> results[i].result = NULL;
      api -> results[i].id = -1;
      api -> results[i].ready = 0;

      pthread_mutex_unlock( & api -> mutex);

      return result;
    }
  }

  pthread_mutex_unlock( & api -> mutex);

  return NULL;
}

API * criarAPI(int n) {
  API * api = (API * ) malloc(sizeof(API));
  api -> n = n;
  api -> buffer.start = 0;
  api -> buffer.end = 0;
  api -> buffer.count = 0;
  pthread_mutex_init( & api -> buffer.mutex, NULL);
  pthread_cond_init( & api -> buffer.cond, NULL);
  int i;

  for (i = 0; i < MAX_RESULT; i++) {
    api -> results[i].result = NULL;
    api -> results[i].id = -1;
    api -> results[i].ready = 0;
    pthread_mutex_init( & api -> results[i].mutex, NULL);
  }

  for (i = 0; i < n; i++) {
    api -> workers[i].id = i + 1;
    api -> workers[i].busy = 0;
    pthread_create( & api -> workers[i].thread, NULL, worker_thread, api);
  }

  pthread_mutex_init( & api -> mutex, NULL);
  pthread_cond_init( & api -> cond, NULL);

  return api;
}

void destruirAPI(API * api) {
  int i;

  for (i = 0; i < api -> n; i++) {
    pthread_cancel(api -> workers[i].thread);
    pthread_join(api -> workers[i].thread, NULL);
  }

  pthread_mutex_destroy( & api -> buffer.mutex);
  pthread_cond_destroy( & api -> buffer.cond);

  for (i = 0; i < MAX_RESULT; i++) {
    pthread_mutex_destroy( & api -> results[i].mutex);
  }

  pthread_mutex_destroy( & api -> mutex);
  pthread_cond_destroy( & api -> cond);

  free(api);
}