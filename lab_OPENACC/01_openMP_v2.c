#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <sys/time.h>

/*
TENTATIVA VERSÃO COM BLOCOS — COMPARE-SPLIT
Cada thread ordena localmente um bloco, e depois executa compare-split com blocos vizinhos.
*/

/*
gcc -fopenmp -O3 -Wall 01_openMP_v2.c -o 01_openMP_v2
./01_openMP_v2 100000 16
*/

// função de comparação para qsort
int compare_int(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

// merge-split menor
void merge_split_low(int local[], int other[], int temp[], int n_local) {
    int i = 0, j = 0, k = 0;
    while (i < n_local && j < n_local) {
        if (local[i] <= other[j]) temp[k++] = local[i++];
        else temp[k++] = other[j++];
    }
    while (i < n_local) temp[k++] = local[i++];
    while (j < n_local) temp[k++] = other[j++];
    memcpy(local, temp, n_local * sizeof(int));
}

// merge-split maior
void merge_split_high(int local[], int other[], int temp[], int n_local) {
    int i = 0, j = 0, k = 0;
    while (i < n_local && j < n_local) {
        if (local[i] <= other[j]) temp[k++] = local[i++];
        else temp[k++] = other[j++];
    }
    while (i < n_local) temp[k++] = local[i++];
    while (j < n_local) temp[k++] = other[j++];
    memcpy(local, temp + n_local, n_local * sizeof(int));
}

// algoritmo principal com blocos
void odd_even_sort_omp_blocos(int *data, int n, int p) {
    int n_local = n / p;

    // cria vetor de ponteiros para blocos
    int **blocos = malloc(p * sizeof(int *));
    for (int i = 0; i < p; ++i)
        blocos[i] = data + i * n_local;

    // ordena localmente (paralelamente)
    #pragma omp parallel for
    for (int i = 0; i < p; ++i)
        qsort(blocos[i], n_local, sizeof(int), compare_int);

    int *temp = malloc(2 * n_local * sizeof(int));
    int *buffer = malloc(n_local * sizeof(int));

    for (int fase = 0; fase < p; ++fase) {
        if (fase % 2 == 0) {
            #pragma omp parallel for private(buffer)
            for (int i = 0; i < p - 1; i += 2) {
                memcpy(buffer, blocos[i + 1], n_local * sizeof(int));
                merge_split_low(blocos[i], buffer, temp, n_local);
                memcpy(buffer, blocos[i], n_local * sizeof(int));
                merge_split_high(blocos[i + 1], buffer, temp, n_local);
            }
        } else {
            #pragma omp parallel for private(buffer)
            for (int i = 1; i < p - 1; i += 2) {
                memcpy(buffer, blocos[i + 1], n_local * sizeof(int));
                merge_split_low(blocos[i], buffer, temp, n_local);
                memcpy(buffer, blocos[i], n_local * sizeof(int));
                merge_split_high(blocos[i + 1], buffer, temp, n_local);
            }
        }
    }

    free(temp);
    free(buffer);
    free(blocos);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <tam vetor> <num threads>\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    int p = atoi(argv[2]);
    omp_set_num_threads(p);

    srand(10416804);
    int *data = malloc(n * sizeof(int));
    for (int i = 0; i < n; ++i)
        data[i] = rand() % 1000 + 1;

    double t0 = omp_get_wtime();
    odd_even_sort_omp_blocos(data, n, p);
    double t1 = omp_get_wtime();

    printf("OpenMP: n = %d, threads = %d -> %f s\n", n, p, t1 - t0);
    free(data);
    return 0;
}
