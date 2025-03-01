#!/bin/bash
set -x  # Ativa a exibição dos comandos usados

# Compilação e execução na ordem original
g++ -O0 -fno-tree-vectorize -fno-inline mult_mat_linhas.cpp -o multMatLinSemOpt
./multMatLinSemOpt

g++ -O3 -march=native mult_mat_linhas.cpp -o multMatLinComOpt
./multMatLinComOpt

g++ -O0 -fno-tree-vectorize -fno-inline mult_mat_colunas.cpp -o multMatColSemOpt
./multMatColSemOpt

g++ -O3 -march=native mult_mat_colunas.cpp -o multMatColComOpt
./multMatColComOpt

g++ -O0 -fno-tree-vectorize -fno-inline mult_mat_blocagem.cpp -o multMatBlocSemOpt
./multMatBlocSemOpt

g++ -O3 -march=native mult_mat_blocagem.cpp -o multMatBlocComOpt
./multMatBlocComOpt

# Análise com Valgrind na mesma sequência
valgrind --tool=cachegrind --cache-sim=yes ./multMatLinSemOpt
valgrind --tool=cachegrind --cache-sim=yes ./multMatLinComOpt
valgrind --tool=cachegrind --cache-sim=yes ./multMatColSemOpt
valgrind --tool=cachegrind --cache-sim=yes ./multMatColComOpt
valgrind --tool=cachegrind --cache-sim=yes ./multMatBlocSemOpt
valgrind --tool=cachegrind --cache-sim=yes ./multMatBlocComOpt

set +x  # Desativa a exibição dos comandos

