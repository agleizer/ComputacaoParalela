// -----------------------------------------------------------------------------
// odd_even_acc_simple.c
// -----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <openacc.h>

// -----------------------------------------------------------------------------
// swap_int(): troca dois inteiros na memória.
// -----------------------------------------------------------------------------
static inline void swap_int(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

// -----------------------------------------------------------------------------
// odd_even_sort_acc:
//   - data: vetor de n inteiros a ordenar (in-place)
//   - n: tamanho do vetor
//   - p: (não usado ativamente, mas colocado para simetria)
// -----------------------------------------------------------------------------
void odd_even_sort_acc(int *data, int n, int p) {
    // Colocamos 'data' toda na GPU no início, e trazemos de volta ao final
    #pragma acc data copy(data[0:n])
    {
        // Fazemos N fases alternando par e ímpar
        for (int phase = 0; phase < n; ++phase) {
            if (phase % 2 == 0) {
                // fase “par”: pares (0-1, 2-3, …)
                #pragma acc parallel loop
                for (int i = 0; i + 1 < n; i += 2) {
                    if (data[i] > data[i+1]) {
                        swap_int(&data[i], &data[i+1]);
                    }
                }
            } else {
                // fase “ímpar”: pares (1-2, 3-4, …)
                #pragma acc parallel loop
                for (int i = 1; i + 1 < n; i += 2) {
                    if (data[i] > data[i+1]) {
                        swap_int(&data[i], &data[i+1]);
                    }
                }
            }
            // sincroniza todos os threads da GPU antes de continuar
            #pragma acc wait
        }
    }
}

// -----------------------------------------------------------------------------
// main(): lê parâmetros, gera vetor, chama a função e mede o tempo.
// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <tamanho_n>\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);  // número de elementos

    // Aloca e preenche o vetor no host
    int *data = malloc(n * sizeof(int));
    for (int i = 0; i < n; ++i) {
        data[i] = rand();
    }

    // Mede tempo (inclui cópia de/para GPU)
    struct timeval tv0, tv1;
    gettimeofday(&tv0, NULL);

    odd_even_sort_acc(data, n, /*p=*/0);

    gettimeofday(&tv1, NULL);
    double elapsed = (tv1.tv_sec - tv0.tv_sec)
                   + (tv1.tv_usec - tv0.tv_usec) * 1e-6;

    printf("OpenACC: n=%d -> %f s\n", n, elapsed);

    free(data);
    return 0;
}
