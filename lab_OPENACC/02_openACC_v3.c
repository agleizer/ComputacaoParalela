#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <openacc.h>

/*
nvc -acc -Minfo=accel -O2 -Wall 02_openACC_v2.c -o 02_openACC_v2
./02_openACC_v2 1000 4
*/

// funcao de comparacao para qsort (ordem crescente)
int comparar_inteiros(const void *a, const void *b) {
    int valor_a = *(const int*)a;
    int valor_b = *(const int*)b;
    return (valor_a < valor_b) ? -1 : (valor_a > valor_b);
}

// funcao auxiliar que determina o balde em que um valor deve ser inserido
int encontrar_balde(int valor, int *splitters, int num_buckets) {
    for (int i = 0; i < num_buckets - 1; ++i) {
        if (valor < splitters[i]) return i;
    }
    return num_buckets - 1;
}

// sample sort paralela com openACC
void sample_sort_acc(int *dados, int tamanho, int num_blocos) {
    int tamanho_bloco = tamanho / num_blocos;

    // ordenacao local e coleta de amostras
    int *amostras = malloc(num_blocos * sizeof(int));
    #pragma omp parallel for
    for (int tid = 0; tid < num_blocos; ++tid) {
        int *bloco = dados + tid * tamanho_bloco;
        qsort(bloco, tamanho_bloco, sizeof(int), comparar_inteiros);
        amostras[tid] = bloco[tamanho_bloco / 2];
    }

    // selecao dos splitters
    qsort(amostras, num_blocos, sizeof(int), comparar_inteiros);
    int *splitters = amostras + 1;  // reaproveita o vetor de amostras

    // alocacao de buffers para baldes
    int *baldes_planos = calloc(tamanho * num_blocos, sizeof(int));
    int *contadores_baldes = calloc(num_blocos, sizeof(int));

    // paralelizacao do particionamento com OpenACC
    #pragma acc data copyin(dados[0:tamanho], splitters[0:num_blocos-1]) \
                     copy(baldes_planos[0:tamanho*num_blocos], contadores_baldes[0:num_blocos])
    {
        #pragma acc parallel loop
        for (int tid = 0; tid < num_blocos; ++tid) {
            int inicio = tid * tamanho_bloco;
            int fim = inicio + tamanho_bloco;
            for (int i = inicio; i < fim; ++i) {
                int valor = dados[i];
                int balde = encontrar_balde(valor, splitters, num_blocos);

                // incremento e escrita atomica
                int pos;
                #pragma acc atomic capture
                pos = contadores_baldes[balde]++;

                baldes_planos[balde * tamanho + pos] = valor;
            }
        }
        #pragma acc wait
    }

    free(amostras);  // splitters esta dentro de amostras

    // 5) ordenacao final dos baldes e reconstrucao do vetor original
    int posicao_escrita = 0;
    for (int b = 0; b < num_blocos; ++b) {
        int qtd = contadores_baldes[b];
        if (qtd > 0) {
            int *inicio_balde = baldes_planos + b * tamanho;
            qsort(inicio_balde, qtd, sizeof(int), comparar_inteiros);
            for (int j = 0; j < qtd; ++j) {
                dados[posicao_escrita++] = inicio_balde[j];
            }
        }
    }

    free(baldes_planos);
    free(contadores_baldes);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <tam vetor> <num BLOCOS>\n", argv[0]);
        return 1;
    }

    int tamanho = atoi(argv[1]);
    int num_blocos = atoi(argv[2]);

    // geracao dos dados
    int *dados = malloc(tamanho * sizeof(int));
    srand(10416804);
    for (int i = 0; i < tamanho; ++i)
        dados[i] = rand() % 1000 + 1;

    /*
    // ordenacao serial com qsort
    int *dados_copia = malloc(tamanho * sizeof(int));
    memcpy(dados_copia, dados, tamanho * sizeof(int));
    double t0 = omp_get_wtime();
    qsort(dados_copia, tamanho, sizeof(int), comparar_inteiros);
    double t1 = omp_get_wtime();
    printf("Tempo Serial:  %.6f s\n", t1 - t0);
    free(dados_copia);
    */

    // ordenacao com OpenACC
    double t2 = omp_get_wtime();
    sample_sort_acc(dados, tamanho, num_blocos);
    double t3 = omp_get_wtime();
    printf("OpenACC: n =%d -> %.6f s\n", tamanho, t3 - t2);

    free(dados);
    return 0;
}
