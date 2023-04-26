#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

typedef struct {
    int largura;
    int altura;
    int valor_max;
    int **pixels;
} ImagemPPM;

typedef struct {
    ImagemPPM *imagem;
    int linha_inicio;
    int linha_fim;
} DadosThread;

void ler_imagem_ppm(const char *nome_arquivo, ImagemPPM *imagem) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    char numero_magico[3];
    fgets(numero_magico, 3, arquivo);
    if (strcmp(numero_magico, "P3") != 0) {
        fprintf(stderr, "Formato inválido\n");
        exit(EXIT_FAILURE);
    }

    fscanf(arquivo, "%d %d %d", &imagem->largura, &imagem->altura, &imagem->valor_max);

    imagem->pixels = (int **) malloc(imagem->altura * sizeof(int *));
    for (int i = 0; i < imagem->altura; i++) {
        imagem->pixels[i] = (int *) malloc(imagem->largura * 3 * sizeof(int));
        for (int j = 0; j < imagem->largura * 3; j++) {
            fscanf(arquivo, "%d", &imagem->pixels[i][j]);
        }
    }

    fclose(arquivo);
}

void salvar_imagem_ppm(const char *nome_arquivo, const ImagemPPM *imagem) {
    FILE *arquivo = fopen(nome_arquivo, "w");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    fprintf(arquivo, "P3\n%d %d\n%d\n", imagem->largura, imagem->altura, imagem->valor_max);

    for (int i = 0; i < imagem->altura; i++) {
        for (int j = 0; j < imagem->largura * 3; j++) {
            fprintf(arquivo, "%d ", imagem->pixels[i][j]);
        }
        fprintf(arquivo, "\n");
    }

    fclose(arquivo);
}

void *converter_tons_cinza(void *arg) {
    DadosThread *dados = (DadosThread *)arg;
    ImagemPPM *imagem = dados->imagem;

    for (int i = dados->linha_inicio; i < dados->linha_fim; i++) {
        for (int j = 0; j < imagem->largura * 3; j += 3) {
            int cinza = imagem->pixels[i][j] * 0.30 + imagem->pixels[i][j + 1] * 0.59 + imagem->pixels[i][j + 2] * 0.11;
            imagem->pixels[i][j] = imagem->pixels[i][j + 1] = imagem->pixels[i][j + 2] = cinza;
        }
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <imagem_entrada.ppm> <imagem_saida.ppm> <numero_threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    ImagemPPM imagem;
    ler_imagem_ppm(argv[1], &imagem);

    int num_threads = atoi(argv[3]);
    pthread_t threads[num_threads];
    DadosThread dados_threads[num_threads];

    int linhas_por_thread = imagem.altura / num_threads;

    for (int i = 0; i < num_threads; i++) {
        dados_threads[i].imagem = &imagem;
        dados_threads[i].linha_inicio = i * linhas_por_thread;
        dados_threads[i].linha_fim = (i == num_threads - 1) ? imagem.altura : (i + 1) * linhas_por_thread;

        pthread_create(&threads[i], NULL, converter_tons_cinza, (void *)&dados_threads[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    salvar_imagem_ppm(argv[2], &imagem);

    for (int i = 0; i < imagem.altura; i++) {
        free(imagem.pixels[i]);
    }
    free(imagem.pixels);

    return 0;
}
