#include <stdio.h>
#include <stdlib.h>
#include "api.h"

void * minhaFuncao(void * param) {
    int * valor = (int *) param;
    printf("Executando minhaFuncao com parametro %d\n", *valor);
    int * resultado = (int *) malloc(sizeof(int));
    *resultado = *valor * 2;
    return resultado;
}

int main() {
    API * api = criarAPI(4);

    int valor1 = 10;
    int valor2 = 20;
    int valor3 = 30;

    int id1 = agendarExecucao(api, minhaFuncao, &valor1);
    int id2 = agendarExecucao(api, minhaFuncao, &valor2);
    int id3 = agendarExecucao(api, minhaFuncao, &valor3);

    void * resultado1 = pegarResultadoExecucao(api, id1);
    void * resultado2 = pegarResultadoExecucao(api, id2);
    void * resultado3 = pegarResultadoExecucao(api, id3);

    printf("Resultado da execucao %d: %d\n", id1, *((int *) resultado1));
    printf("Resultado da execucao %d: %d\n", id2, *((int *) resultado2));
    printf("Resultado da execucao %d: %d\n", id3, *((int *) resultado3));

    free(resultado1);
    free(resultado2);
    free(resultado3);

    destruirAPI(api);

    return 0;
}
