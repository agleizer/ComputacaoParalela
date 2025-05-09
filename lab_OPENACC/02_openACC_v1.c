// ==================================================================================
// sample_sort_acc_simple.c
// ==================================================================================
// Exemplo didático de Sample Sort em “blocos” usando OpenACC,
// com comparador correto e fases simplificadas.
// ==================================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <openacc.h>
#include <omp.h>   // para usar omp_get_wtime()

// -----------------------------------------------------------------------------
// cmp_int(): comparador para qsort de inteiros em ordem crescente.
// -----------------------------------------------------------------------------
static int cmp_int(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia < ib) ? -1 : (ia > ib) ? 1 : 0;
}

// -----------------------------------------------------------------------------
// find_bucket(): determina em qual balde x se encaixa.
// -----------------------------------------------------------------------------
static int find_bucket(int x, int *splitters, int p) {
    for (int b = 0; b < p-1; ++b) {
        if (x < splitters[b]) return b;
    }
    return p-1;
}

// -----------------------------------------------------------------------------
// sample_sort_acc(): fases principais do Sample Sort com OpenACC.
// -----------------------------------------------------------------------------
void sample_sort_acc(int *data, int n, int p) {
    int n_local = n / p;

    // 1) host: ordenação local e coleta de amostras
    int *samples   = malloc(p * sizeof(int));
    int *splitters = malloc((p-1) * sizeof(int));
    #pragma omp parallel for
    for (int tid = 0; tid < p; ++tid) {
        qsort(data + tid*n_local, n_local, sizeof(int), cmp_int);
        samples[tid] = data[tid*n_local + n_local/2];
    }

    // 2) host: seleção de splitters
    qsort(samples, p, sizeof(int), cmp_int);
    for (int i = 0; i < p-1; ++i) {
        splitters[i] = samples[i+1];
    }
    free(samples);

    // 3) prepara buffers para particionamento
    int *buckets_flat  = calloc(p * n, sizeof(int));
    int *bucket_counts = calloc(p, sizeof(int));

    // 4) offload do particionamento para GPU
    #pragma acc data copyin(data[0:n], splitters[0:p-1]) \
                     copy(buckets_flat[0:p*n], bucket_counts[0:p])
    {
        #pragma acc parallel loop
        for (int tid = 0; tid < p; ++tid) {
            int start = tid * n_local;
            int end   = start + n_local;
            for (int i = start; i < end; ++i) {
                int x = data[i];
                int b = find_bucket(x, splitters, p);
                // atomically incrementa e escreve no bucket
                #pragma acc atomic update
                bucket_counts[b]++;
                int pos = bucket_counts[b] - 1;
                buckets_flat[b*n + pos] = x;
            }
        }
        #pragma acc wait
    }
    free(splitters);

    // 5) host: ordena cada balde e reagrupa em data[]
    int pos = 0;
    for (int b = 0; b < p; ++b) {
        int cnt = bucket_counts[b];
        if (cnt > 0) {
            qsort(buckets_flat + b*n, cnt, sizeof(int), cmp_int);
            for (int j = 0; j < cnt; ++j) {
                data[pos++] = buckets_flat[b*n + j];
            }
        }
    }

    free(buckets_flat);
    free(bucket_counts);
}

// -----------------------------------------------------------------------------
// main(): lê n, p, seed; mede tempo serial vs. paralelo OpenACC.
// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr,
            "Uso: %s <n> <p> <seed>\n"
            "  n    = tamanho do vetor\n"
            "  p    = nº de blocos\n"
            "  seed = semente rand()\n",
            argv[0]);
        return 1;
    }
    int n    = atoi(argv[1]);
    int p    = atoi(argv[2]);
    int seed = atoi(argv[3]);

    int *data = malloc(n * sizeof(int));
    srand(seed);
    for (int i = 0; i < n; ++i) data[i] = rand();

    // -- serial --
    int *tmp = malloc(n * sizeof(int));
    memcpy(tmp, data, n * sizeof(int));
    double ts0 = omp_get_wtime();
    qsort(tmp, n, sizeof(int), cmp_int);
    double ts1 = omp_get_wtime();
    printf("Serial: %f s\n", ts1 - ts0);
    free(tmp);

    // -- paralelo OpenACC --
    double t0 = omp_get_wtime();
    sample_sort_acc(data, n, p);
    double t1 = omp_get_wtime();
    printf("OpenACC: %f s\n", t1 - t0);

    free(data);
    return 0;
}
