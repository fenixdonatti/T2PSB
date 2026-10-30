/*
    main.c - Programa de testes do alocador de memoria

    Este arquivo não implementa o alocador: ele apenas usa as funções
    declaradas em mymemory.h para verificar se elas funcionam.

    Uso de IA:
        - Foi utilizado o Claude para gerar os valores do size_t.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mymemory.c"

#define NUM_BLOCOS 50

static const size_t TAMANHOS[NUM_BLOCOS] = {
    10, 25, 50, 100, 250, 500, 1000,
    12, 30, 60, 128, 300, 512,  900,
     8, 20, 45,  90, 200, 450,  750,
    16, 35, 64, 150, 256, 600, 1024,
     5, 40, 70, 110, 220, 350,  800,
    18, 28, 55,  95, 180, 420,  650,
    22, 32, 80, 140, 275, 380,  700,
    2048
};

// Contador de falhas
static int falhas = 0;