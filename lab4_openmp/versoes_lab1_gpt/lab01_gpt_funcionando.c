#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

// Função a ser integrada
double f(double x)
{
    return sin(x);
}

/*
 * Função que calcula a integral de f(x) no intervalo [a, b] usando a regra do trapézio.
 * Utilizamos OpenMP para paralelizar o cálculo e melhorar a eficiência.
 * Cada thread calcula um subintervalo exclusivo para evitar somas incorretas.
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
double Local_trap(double a, double b, int n, int thread_count)
{
    double h, local_a, local_b, my_result = 0.0;
    int i, local_n;
    int my_rank = omp_get_thread_num();

    // Calculamos o tamanho de cada trapézio
    h = (b - a) / n;

    // Cada thread calcula um intervalo separado
    local_n = n / thread_count;
    local_a = a + my_rank * local_n * h;
    local_b = local_a + local_n * h;

    // Calculamos a área dentro do intervalo de cada thread
    my_result = (f(local_a) + f(local_b)) / 2.0;

    for (i = 1; i <= local_n - 1; i++)
    {
        my_result += f(local_a + i * h);
    }

    my_result *= h;

    return my_result;
}

int main(int argc, char *argv[])
{
    double a, b, global_result = 0.0;
    int n, thread_count;

    // Verificação de entrada correta dos argumentos
    if (argc != 2)
    {
        printf("Uso: %s <número de threads>\n", argv[0]);
        return 1;
    }

    // Obtém o número de threads da linha de comando
    thread_count = strtol(argv[1], NULL, 10);

    // Solicita ao usuário os valores dos limites e o número de trapézios
    printf("Digite os valores de a, b e n: ");
    scanf("%lf %lf %d", &a, &b, &n);

/*
 * Agora utilizamos "parallel" e "reduction" corretamente.
 * Cada thread chama Local_trap() com um subintervalo separado,
 * garantindo que os cálculos não sejam duplicados.
 */
#pragma omp parallel num_threads(thread_count) reduction(+ : global_result)
    global_result += Local_trap(a, b, n, thread_count);

    // Exibe o resultado final da integral aproximada
    printf("Com n = %d trapézios, a estimativa da integral de %f a %f = %.14e\n", n, a, b, global_result);

    return 0;
}
