// -----------------------------------------------------------------------------
// odd_even_omp_simple.c
// -----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

// -----------------------------------------------------------------------------
// swap_int(): troca dois inteiros na memória.
// -----------------------------------------------------------------------------
static inline void swap_int(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

// -----------------------------------------------------------------------------
// odd_even_sort_omp:
//   - data: vetor de n inteiros a ordenar (in-place)
//   - n: tamanho do vetor
//   - p: número de threads (definido em main via omp_set_num_threads)
// -----------------------------------------------------------------------------
void odd_even_sort_omp(int *data, int n, int p) {
    // O algoritmo faz N fases (onde N = n):
    // em cada fase alternamos “fase par” e “fase ímpar”
    for (int phase = 0; phase < n; ++phase) {
        if (phase % 2 == 0) {
            // fase “par”: comparamos pares (0-1, 2-3, 4-5, …)
            #pragma omp parallel for
            for (int i = 0; i + 1 < n; i += 2) {
                if (data[i] > data[i+1]) {
                    swap_int(&data[i], &data[i+1]);
                }
            }
        } else {
            // fase “ímpar”: comparamos pares (1-2, 3-4, 5-6, …)
            #pragma omp parallel for
            for (int i = 1; i + 1 < n; i += 2) {
                if (data[i] > data[i+1]) {
                    swap_int(&data[i], &data[i+1]);
                }
            }
        }
        // NOTA: o "#pragma omp parallel for" já sincroniza automaticamente
        // cada loop antes de avançar para a próxima fase.
    }
}

// -----------------------------------------------------------------------------
// main(): lê parâmetros, gera vetor, chama a função e mede o tempo.
// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <tamanho_n> <n_threads>\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);        // número de elementos
    int p = atoi(argv[2]);        // número de threads

    // Setamos o número de threads **antes** de chamar a função
    omp_set_num_threads(p);

    // Aloca e preenche o vetor com valores aleatórios
    int *data = malloc(n * sizeof(int));
    for (int i = 0; i < n; ++i) {
        data[i] = rand();
    }

    // Mede tempo de execução
    double t0 = omp_get_wtime();
    odd_even_sort_omp(data, n, p);
    double t1 = omp_get_wtime();

    printf("OpenMP: n=%d, threads=%d -> %f s\n", n, p, t1 - t0);

    free(data);
    return 0;
}
