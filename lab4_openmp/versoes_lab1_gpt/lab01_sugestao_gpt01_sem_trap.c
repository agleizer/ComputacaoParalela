#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

/* Função a ser integrada: sin(x), mas pode trocar */
double f(double x) { return sin(x); }

int main(int argc, char *argv[])
{
    /* Variáveis de entrada */
    double a, b;
    int n, thread_count;

    /* Lê o número de threads pela linha de comando */
    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s <n_threads>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    thread_count = strtol(argv[1], NULL, 10);

    /* Lê a, b, n via stdin */
    printf("Entre com a, b e n:\n");
    scanf("%lf %lf %d", &a, &b, &n);

    /* Cálculo da Regra do Trapézio */
    double h = (b - a) / n;
    double approx = (f(a) + f(b)) / 2.0;

#pragma omp parallel for num_threads(thread_count) reduction(+ : approx)
    for (int i = 1; i <= n - 1; i++)
    {
        approx += f(a + i * h);
    }

    approx *= h;

    printf("Com n=%d trapezios, estimativa da integral de %f a %f = %.14e\n", n,
           a, b, approx);

    return 0;
}
