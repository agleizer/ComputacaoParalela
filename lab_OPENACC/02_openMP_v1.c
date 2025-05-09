// ==================================================================================
// sample_sort_omp_simple.c
// ==================================================================================
// Exemplo didático de Sample Sort em “blocos” usando OpenMP.
// Cada thread:
//  1) ordena localmente seu bloco com qsort()
//  2) escolhe UMA amostra (o elemento do meio do bloco)
// Depois:
//  3) a thread 0 coleta todas as amostras, ordena-as e escolhe p–1 splitters
//  4) cada thread particiona seu bloco em p “baldes” usando esses splitters
//  5) as threads escrevem seus baldes num buffer global (com critical)
//  6) a thread 0 faz qsort em cada balde final e reescreve data[] com eles
//  
// Para iniciantes: não é a forma mais eficiente de all-to-all, mas demonstra as fases!
// ==================================================================================

// -----------------------------------------------------------------------------
// sample_sort_omp_simple.c
// Exemplo didático de Sample Sort em “blocos” usando OpenMP,
// com comparador correto para inteiros.
// -----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <sys/time.h>

// -----------------------------------------------------------------------------
// cmp_int(): comparador para qsort de inteiros em ordem crescente.
// -----------------------------------------------------------------------------
static int cmp_int(const void *a, const void *b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia < ib) ? -1 : (ia > ib) ? 1 : 0;
}

// -----------------------------------------------------------------------------
// find_bucket(): determina em qual "balde" x se encaixa, dado splitters[0..p-2].
// -----------------------------------------------------------------------------
static int find_bucket(int x, int *splitters, int p) {
    for (int b = 0; b < p-1; ++b) {
        if (x < splitters[b]) return b;
    }
    return p-1;
}

// -----------------------------------------------------------------------------
// sample_sort_omp(): implementa Sample Sort bloco-level em OpenMP.
// -----------------------------------------------------------------------------
void sample_sort_omp(int *data, int n, int p) {
    int n_local   = n / p;
    int *samples  = malloc(p * sizeof(int));     // uma amostra por bloco
    int *splitters= malloc((p-1) * sizeof(int)); // p-1 splitters

    // 1) ordenação local e coleta de uma amostra
    #pragma omp parallel num_threads(p)
    {
        int tid   = omp_get_thread_num();
        int *blk  = data + tid * n_local;
        qsort(blk, n_local, sizeof(int), cmp_int);           // ordena bloco
        samples[tid] = blk[n_local/2];                       // amostra do meio
    }

    // 2) seleção centralizada de splitters (thread única)
    qsort(samples, p, sizeof(int), cmp_int);
    for (int i = 0; i < p-1; ++i) {
        splitters[i] = samples[i+1];
    }
    free(samples);

    // 3) particionamento local em buckets
    int *buckets_flat  = calloc(p * n, sizeof(int)); // espaço para todos os baldes
    int *bucket_counts = calloc(p, sizeof(int));

    #pragma omp parallel for
    for (int tid = 0; tid < p; ++tid) {
        int *blk = data + tid * n_local;
        for (int j = 0; j < n_local; ++j) {
            int x = blk[j];
            int b = find_bucket(x, splitters, p);
            #pragma omp critical
            {
                int idx = bucket_counts[b]++;
                buckets_flat[b*n + idx] = x;
            }
        }
    }
    free(splitters);

    // 4) ordenação final de cada balde e reconstrução de data[]
    int pos = 0;
    for (int b = 0; b < p; ++b) {
        int cnt = bucket_counts[b];
        if (cnt > 0) {
            qsort(buckets_flat + b*n, cnt, sizeof(int), cmp_int);
            for (int k = 0; k < cnt; ++k) {
                data[pos++] = buckets_flat[b*n + k];
            }
        }
    }

    free(buckets_flat);
    free(bucket_counts);
}

// -----------------------------------------------------------------------------
// main(): lê n, p, seed; mede tempo serial vs. paralelo OpenMP.
// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr,
            "Uso: %s <n> <p> <seed>\n"
            "  n    = tamanho do vetor\n"
            "  p    = nº de threads/blocos\n"
            "  seed = semente de rand()\n",
            argv[0]);
        return 1;
    }
    int n    = atoi(argv[1]);
    int p    = atoi(argv[2]);
    int seed = atoi(argv[3]);

    int *data = malloc(n * sizeof(int));
    srand(seed);
    for (int i = 0; i < n; ++i) data[i] = rand();

    // serial (total com qsort)
    int *tmp = malloc(n * sizeof(int));
    memcpy(tmp, data, n * sizeof(int));
    double t0 = omp_get_wtime();
    qsort(tmp, n, sizeof(int), cmp_int);
    double t1 = omp_get_wtime();
    printf("Serial:  %f s\n", t1 - t0);
    free(tmp);

    // paralelo OpenMP
    double t2 = omp_get_wtime();
    sample_sort_omp(data, n, p);
    double t3 = omp_get_wtime();
    printf("OpenMP: %f s\n", t3 - t2);

    free(data);
    return 0;
}
