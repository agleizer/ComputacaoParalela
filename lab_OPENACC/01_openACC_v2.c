#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <openacc.h>

/*
VERSAO COM BLOCOS — COMPARE-SPLIT
Cada "bloco" é ordenado localmente na CPU com qsort. O compare-split é feito na GPU.
*/

/*
nvc -acc -Minfo=accel -O3 -Wall 01_openACC_v2.c -o 01_openACC_v2
./01_openACC_v2 100000 4
*/

// comparacao para qsort
int compare_int(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// versao com blocos e OpenACC
void odd_even_sort_acc_blocos(int *data, int n, int p) {
    int n_local = n / p;

    // ordenacao local em CPU
    for (int i = 0; i < p; ++i) {
        qsort(data + i * n_local, n_local, sizeof(int), compare_int);
    }

    // envio para GPU
    #pragma acc data copy(data[0:n])
    {
        int fase;
        for (fase = 0; fase < p; ++fase) {
            if (fase % 2 == 0) {
                #pragma acc parallel loop
                for (int i = 0; i < p - 1; i += 2) {
                    int *local = &data[i * n_local];
                    int *other = &data[(i + 1) * n_local];
                    int temp[2048]; // máximo: 2*n_local
                    int li = 0, ri = 0, k = 0;
                    while (li < n_local && ri < n_local)
                        temp[k++] = (local[li] <= other[ri]) ? local[li++] : other[ri++];
                    while (li < n_local) temp[k++] = local[li++];
                    while (ri < n_local) temp[k++] = other[ri++];
                    for (int j = 0; j < n_local; ++j)
                        local[j] = temp[j];
                    for (int j = 0; j < n_local; ++j)
                        other[j] = temp[j + n_local];
                }
            } else {
                #pragma acc parallel loop
                for (int i = 1; i < p - 1; i += 2) {
                    int *local = &data[i * n_local];
                    int *other = &data[(i + 1) * n_local];
                    int temp[2048];
                    int li = 0, ri = 0, k = 0;
                    while (li < n_local && ri < n_local)
                        temp[k++] = (local[li] <= other[ri]) ? local[li++] : other[ri++];
                    while (li < n_local) temp[k++] = local[li++];
                    while (ri < n_local) temp[k++] = other[ri++];
                    for (int j = 0; j < n_local; ++j)
                        local[j] = temp[j];
                    for (int j = 0; j < n_local; ++j)
                        other[j] = temp[j + n_local];
                }
            }
            #pragma acc wait
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <tam vetor> <num blocos>\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    int p = atoi(argv[2]);

    srand(10416804);
    int *data = malloc(n * sizeof(int));
    for (int i = 0; i < n; ++i)
        data[i] = rand() % 1000 + 1;

    struct timeval tv0, tv1;
    gettimeofday(&tv0, NULL);
    odd_even_sort_acc_blocos(data, n, p);
    gettimeofday(&tv1, NULL);

    double tempo = (tv1.tv_sec - tv0.tv_sec) + (tv1.tv_usec - tv0.tv_usec) * 1e-6;
    printf("OpenACC: n = %d, blocos = %d -> %f s\n", n, p, tempo);
    free(data);
    return 0;
}
