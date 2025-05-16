/*
Universidade Presbiteriana Mackenzie
Computação Paralela - Turma 05P11

Alan Meniuk Gleizer
RA 10416804
16/05/2025

(sem dupla)
*/

/*
Lab 1 - Programação - Integral Regra do Trapézio (em aula)
>>> REIMPLEMENTAÇÃO EM OPENACC <<<
*/

#include <stdio.h>
#include <stdlib.h>
#include <openacc.h>
#include <math.h>

/*
PARA COMPILAR
nvc -acc -Minfo=accel -fast lab01_openACC.c -o lab01_openACC -lm

PARA EXECUTAR
./lab01_openACC
*/

double f(double a)
{
    return sin(a);
}

double Local_trap(double a, double b, int n)
{
    double h = (b - a) / n;
    double approx = (f(a) + f(b)) / 2.0;
    int i;

    // aqui estamos paralelizando o for loop
    // uso de reduction NO LUGAR DE CRITICAL
    #pragma acc parallel loop reduction(+ : approx)
    for (i = 1; i <= n - 1; i++)
        approx += f(a + i * h);

    return h * approx;
}

int main(int argc, char *argv[])
{
    double global_result = 0.0; /* Store result in global_result */
    double a, b;                /* Left and right endpoints */
    int n;                      /* Total number of trapezoids */
    // int thread_count; // não sera usado em openACC

    // thread_count = strtol(argv[1], NULL, 10);
    printf("Enter a, b, and n\n");
    scanf("%lf %lf %d", &a, &b, &n);

    global_result += Local_trap(a, b, n);

    printf("With n = %d trapezoids, our estimate\n", n);
    printf("of the integral from %f to %f = %.14e\n", a, b, global_result);
    return 0;
} /* main */
