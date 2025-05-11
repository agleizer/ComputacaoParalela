/*
Lab 1 - Programação - Integral Regra do Trapézio (em aula)

Faça o código da integral pela regra do trapézio mostrado nos slides de Introdução ao OpenMP (

04 - Programacao de Memoria Compartilhada com OpenMP - ch5), na página 24.
Implemente as diversas melhorias no seu código, até chegar na página 38 (com **reduction** )

Submeta o seu código, com o seu nome no início do código em comentário.

Submeta prints da execução do código.

PARA COMPILAR
gcc -g -Wall -fopenmp lab01_refslides.c -o original -lm
gcc -g -Wall -fopenmp main.c -o original -lm
*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

void Trap(double a, double b, int n, double *global_result_p);

double f(double a)
{
    return sin(a);
}

int main(int argc, char *argv[])
{
    double global_result = 0.0; /* Store result in global_result */
    double a, b;                /* Left and right endpoints */
    int n;                      /* Total number of trapezoids */
    int thread_count;

    thread_count = strtol(argv[1], NULL, 10);
    printf("Enter a, b, and n\n");
    scanf("%lf %lf %d", &a, &b, &n);
#pragma omp parallel num_threads(thread_count)
    Trap(a, b, n, &global_result);

    printf("With n = %d trapezoids, our estimate\n", n);
    printf("of the integral from %f to %f = %.14e\n", a, b, global_result);
    return 0;
} /* main */

void Trap(double a, double b, int n, double *global_result_p)
{
    double h, x, my_result;
    double local_a, local_b;
    int i, local_n;
    int my_rank = omp_get_thread_num();
    int thread_count = omp_get_num_threads();

    h = (b - a) / n;
    local_n = n / thread_count;
    local_a = a + my_rank * local_n * h;
    local_b = local_a + local_n * h;
    my_result = (f(local_a) + f(local_b)) / 2.0; // f é a função que queremos integrar
    for (i = 1; i <= local_n - 1; i++)
    {
        x = local_a + i * h;
        my_result += f(x);
    }
    my_result = my_result * h;

#pragma omp critical
    *global_result_p += my_result;
} /* Trap. */