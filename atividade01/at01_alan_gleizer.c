/*
Universidade Presbiteriana Mackenzie
Ciência da Computação – 05P
Computação Paralela
18/02/2025
Alan Meniuk Gleizer
RA 10416804
*/

/* Exercicio 01
Implemente um programa utilizando a chamada de função wait() que espere todos
os n processos filhos terminarem, e imprima na tela o PID e o código de retorno
de cada um. O processo pai não deve guardar os PIDs dos filhos criados, e deve
mostrar, no mínimo, a seguinte saída:

$ ./exercicio1
No processo pai: processo filho: 64464, status de saída do filho: 1
No processo pai: processo filho: 64463, status de saída do filho: 3
No processo pai: processo filho: 64462, status de saída do filho: 8
No processo pai: processo filho: 64465, status de saída do filho: 10

O código de retorno dos filhos deve ser o tempo que ficam em sleep; você pode utilizar
um tempo aleatório para o sleep da seguinte forma:

#include <stdlib.h>
#include <time.h>
...
srand(time(NULL)+getpid());
sleep_time = rand() % 10 + 1;
sleep(sleep_time);
...

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int lerInt()
{
    // funcao da biblioteca propria para ler um int sem os problemas de scanf
    char buffer[32]; // buffer para input
    int num;

    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
    {
        return 0; // se fgets é NULL, houve erro de leitura ou EOF
    }

    if (sscanf(buffer, "%d", &num) != 1)
    {
        return 0; // verificar se entrada realmente é int
    }

    return num;
}

int mainOld()
{
    printf("Informe a quantidade de filhos: ");
    int totalFilhos = lerInt();

    // varivaeis que serão usadas por todos os processos, pai e filhos
    int pid;
    int status; // atualizado automaticamento pelo SO
    int numFilhos = 0;

    // loop para fork cada filho

    while (numFilhos < totalFilhos)
    {
        pid = fork();

        // processo filho (fork retorna 0 para o filho!)
        if (pid == 0)
        {
            printf("Executando filho...\n");
            srand(time(NULL) + getpid());
            int sleep_time = rand() % 10 + 1;
            sleep(sleep_time);
            return sleep_time;
        }
        else
        {
            // processo pai
            wait(&status); // kernel atualiza status com saída do filho
            printf("No processo pai: processo filho: %d, status de saída do filho: %d\n", pid, WEXITSTATUS(status));
            numFilhos++;
        }
    }

    return 0;
}

int main()
{
    printf("Informe a quantidade de filhos: ");
    int totalFilhos = 0;
    do
    {
        totalFilhos = lerInt();
    } while (totalFilhos < 1);

    int numFilhos = 0;

    // Criar todos os filhos primeiro
    for (int i = 0; i < totalFilhos; i++)
    {
        pid_t pid = fork();

        if (pid < 0)
        {
            printf("Erro ao criar processo filho");
            return (-1);
        }

        if (pid == 0)
        {
            // Processo filho
            srand(time(NULL) + getpid());
            int sleep_time = rand() % 10 + 1;
            sleep(sleep_time);
            return (sleep_time);
        }
    }

    // Processo pai espera todos os filhos terminarem
    for (int i = 0; i < totalFilhos; i++)
    {
        int status;
        pid_t child_pid = wait(&status);
        /*
        wait() bloqueia execução do pai até que QUALQUER filho termine
        retorno de wait é o PID do filho que terminou.. por isso não precisamos armazenar o PID
        o SO gerencia isso automaticamente
        */

        if (WIFEXITED(status))
        {
            printf("No processo pai: processo filho: %d, status de saída do filho: %d\n", child_pid, WEXITSTATUS(status));
        }
        else
        {
            printf("No processo pai: processo filho: %d terminou anormalmente.\n", child_pid);
        }
    }

    return 0;
}