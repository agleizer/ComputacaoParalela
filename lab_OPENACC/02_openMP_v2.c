#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

/*
gcc -fopenmp -O3 -Wall 02_openMP_v2.c -o 02_openMP_v2
./02_openMP_v2 1000 16
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

// sample sort paralela com openMP
void sample_sort_omp(int *dados, int tamanho, int num_threads) {
    int tamanho_bloco = tamanho / num_threads;

    // vetor para armazenar uma amostra de cada bloco
    int *amostras = malloc(num_threads * sizeof(int));

    // ordenacao local e selecao de uma amostra de cada bloco
    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();
        int *bloco_local = dados + tid * tamanho_bloco;

        // ordena localmente o bloco
        qsort(bloco_local, tamanho_bloco, sizeof(int), comparar_inteiros);

        // escolhe o elemento do meio como amostra representativa
        amostras[tid] = bloco_local[tamanho_bloco / 2];
    }

    // selecao dos splitters (feita pela thread 0)
    qsort(amostras, num_threads, sizeof(int), comparar_inteiros);

    // usar amostras[1..p-1] como splitters
    int *splitters = amostras + 1;

    // particionamento local nos baldes globais
    int *baldes_planos = calloc(tamanho * num_threads, sizeof(int));  // espaco para todos os elementos em cada balde
    int *contadores_baldes = calloc(num_threads, sizeof(int));        // contagem de elementos por balde

    #pragma omp parallel for
    for (int tid = 0; tid < num_threads; ++tid) {
        int *bloco_local = dados + tid * tamanho_bloco;
        for (int j = 0; j < tamanho_bloco; ++j) {
            int valor = bloco_local[j];
            int balde = encontrar_balde(valor, splitters, num_threads);

            // insere o valor no balde correspondente (posicao protegida por atomic)
            int posicao;
            #pragma omp atomic capture
            posicao = contadores_baldes[balde]++;

            baldes_planos[balde * tamanho + posicao] = valor;
        }
    }

    // ordenacao final dos baldes e reconstrucao do vetor original
    int posicao_escrita = 0;
    for (int b = 0; b < num_threads; ++b) {
        int quantidade = contadores_baldes[b];
        if (quantidade > 0) {
            int *inicio_balde = baldes_planos + b * tamanho;

            // ordena os elementos dentro do balde
            qsort(inicio_balde, quantidade, sizeof(int), comparar_inteiros);

            // copia os elementos ordenados para o vetor final
            for (int k = 0; k < quantidade; ++k) {
                dados[posicao_escrita++] = inicio_balde[k];
            }
        }
    }

    free(amostras);
    free(baldes_planos);
    free(contadores_baldes);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <tam vetor> <num threads>\n", argv[0]);
        return 1;
    }

    int tamanho = atoi(argv[1]);
    int num_threads = atoi(argv[2]);

    // geracao dos dados
    int *dados = malloc(tamanho * sizeof(int));
    srand(10416804);
    for (int i = 0; i < tamanho; ++i) dados[i] = rand() % 1000 + 1;

    // medida de tempo para qsort serial
    /*
    int *dados_copia = malloc(tamanho * sizeof(int));
    memcpy(dados_copia, dados, tamanho * sizeof(int));

    double t0 = omp_get_wtime();
    qsort(dados_copia, tamanho, sizeof(int), comparar_inteiros);
    double t1 = omp_get_wtime();
    printf("Tempo Serial:   %.6f s\n", t1 - t0);
    free(dados_copia);
    */

    // medida de tempo para sample sort com OpenMP
    double t2 = omp_get_wtime();
    sample_sort_omp(dados, tamanho, num_threads);
    double t3 = omp_get_wtime();
    printf("OpenMP: n = %d, threads = %d -> %.6f s\n", tamanho, num_threads, t3 - t2);

    free(dados);
    return 0;
}
