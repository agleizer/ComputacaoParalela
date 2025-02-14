#include <stdio.h>
#include <unistd.h>

// exemplo simples de fork
// ao fazer um fork, o filho recebe uma cópia exata do espaço de endereçamento do pai. ele recebe todo o histórico anterior.. todas as variáveis etc.
// 

int main(void) {
   int x;

   x = 0;
   fork();
   x = 1;
   printf("I am process %ld and my x is %d\n", (long)getpid(), x);
   return 0;
}