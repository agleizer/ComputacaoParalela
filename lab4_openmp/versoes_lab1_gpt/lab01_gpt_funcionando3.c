#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

// Função a ser integrada
double f(double x) { return sin(x); }

/*
 * Função que calcula a integral de f(x) no intervalo [a, b] usando a regra do
 * trapézio. Utilizamos OpenMP para paralelizar o cálculo e melhorar a
 * eficiência.
 *
 * Parâmetros:
 * - a: limite inferior da integral
 * - b: limite superior da integral
 * - n: número de trapézios utilizados na aproximação
 * - thread_count: número de threads utilizadas na paralelização
 *
 * Retorna:
 * - O valor aproximado da integral.
 */
double Trap(double a, double b, int n, int thread_count)
{
    double h, global_result = 0.0;
    int i;

    // Calculamos o tamanho de cada trapézio
    h = (b - a) / (double)n;

/*
 * Agora utilizamos a diretiva "parallel" para dividir o trabalho entre as
 * threads. Cada thread calcula um subintervalo exclusivo, evitando cálculos
 * duplicados. Usamos "reduction(+: global_result)" para acumular corretamente
 * os resultados.
 */
#pragma omp parallel num_threads(thread_count) reduction(+ : global_result)
    {
        double local_a, local_b, my_result;
        int local_n = n / thread_count;
        int my_rank = omp_get_thread_num();

        local_a = a + my_rank * local_n * h;
        local_b = local_a + local_n * h;

        my_result = (f(local_a) + f(local_b)) / 2.0;

        for (i = 1; i <= local_n - 1; i++)
        {
            my_result += f(local_a + i * h);
        }

        my_result *= h;
        global_result += my_result;
    }

    return global_result;
}

int main(int argc, char *argv[])
{
    double a, b, global_result;
    int n, thread_count;

    // Verificação de entrada correta dos argumentos
    if (argc != 2)
    {
        printf("Uso: %s <número de threads>\n", argv[0]);
        return 1;
    }

    // Obtém o número de threads da linha de comando
    thread_count = strtol(argv[1], NULL, 10);
    omp_set_num_threads(thread_count);

    // Solicita ao usuário os valores dos limites e o número de trapézios
    printf("Digite os valores de a, b e n: ");
    scanf("%lf %lf %d", &a, &b, &n);

    /*
     * Chamamos a função Trap() com o número correto de threads,
     * garantindo que a integral seja calculada paralelamente e corretamente
     * acumulada.
     */
    global_result = Trap(a, b, n, thread_count);

    // Exibe o resultado final da integral aproximada
    printf("Com n = %d trapézios, a estimativa da integral de %f a %f = %.14e\n",
           n, a, b, global_result);

    return 0;
}
