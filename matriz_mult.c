#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_t t1, t2, t3;

typedef struct{
    int tam_linha;
    int* linhas;
    int tam_coluna;
    int* colunas;
    char id;
}Dado;

Dado *criar_matriz(int l, int c){
    Dado matriz;
    matriz.linhas=NULL;
    matriz.colunas=NULL;
    matriz.tam_linha = l;
    matriz.tam_coluna = c;
    matriz.linhas = (int*)malloc(sizeof(int) * matriz.tam_linha);
    matriz.colunas = (int*)malloc(sizeof(int) * matriz.tam_coluna);
    return &matriz;
}

Dado *defin_lc(int l, int c){
    printf("defina a quatidade de linhas: ");
    scanf("%d", &l);
    printf("defina a quatidade de linhas: ");
    scanf("%d", &c);
    return criar_matriz(l, c);
}


void preencher_linhas(Dado mtz){
    for(int i=0; i<mtz.tam_linha; i++){
        mtz.linhas[i] * 2;
    }
}

void calcLinha(Dado mtz){
    for(int i=0; i<mtz.tam_coluna; i++){
        mtz.colunas[i] * 2;
    }
}

int main(){
    int linhas, colunas;
    Dado matriz;
    pthread_create(&t1, NULL, (void*)defin_lc, NULL);
    pthread_join(t1, (void**)&matriz);

    printf("%d %d", matriz.tam_linha, matriz.tam_coluna);

    return 0;
}