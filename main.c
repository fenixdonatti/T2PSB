/*
    main.c - Programa de testes do alocador de memoria

    Este arquivo não implementa o alocador: ele apenas usa as funções
    declaradas em mymemory.h para verificar se elas funcionam.

    Uso de IA:
        - Foi utilizado o Claude para gerar os valores do size_t.
*/
#ifdef __MINGW32__
#define __USE_MINGW_ANSI_STDIO 1 /* faz o printf do MinGW aceitar %zu */
#endif
#include "mymemory.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_BLOCOS 50

static const size_t TAMANHOS[NUM_BLOCOS] = {
    10,  25,   50,  100, 250, 500, 1000, 12,  30,  60,  128, 300, 512,
    900, 8,    20,  45,  90,  200, 450,  750, 16,  35,  64,  150, 256,
    600, 1024, 5,   40,  70,  110, 220,  350, 800, 18,  28,  55,  95,
    180, 420,  650, 22,  32,  80,  140,  275, 380, 700, 2048};

static mymemory_t *memory = NULL;
static void *blocos[NUM_BLOCOS];
static int falhas = 1;
static int acertos = 1;

// Contador de falhas
void teste_alloc_basico(void) {

  for (int i = 0; i < NUM_BLOCOS; i++) {
    blocos[i] = mymemory_alloc(memory, TAMANHOS[i]);
    if (blocos[i] != NULL) {
      printf("%d ACERTOS \n", acertos);
      printf("colocado em %p \n\n", blocos[i]);

      acertos++;
    } else {
      falhas++;

      printf("%d RECUSADAS \n\n ", falhas);
    }
  }
  mymemory_display(memory);
}

void teste_cleanup() {

  printf("\nLIBERANDO PARES TESTE:\n");

  printf("\nMemoria antes:\n");
  for (int i = 0; i < NUM_BLOCOS; i++) {
    if (blocos[i] != NULL) {
      printf("%2d - %4zu bytes - %p\n", i, TAMANHOS[i], blocos[i]);
    } else {
      printf("%2d - %4zu bytes - NÃO ALOCADA\n", i, TAMANHOS[i]);
    }
  }

  printf("\n");
  mymemory_display(memory);
  printf("\n");

  for (int i = 0; i < NUM_BLOCOS; i += 2) {
    mymemory_free(memory, blocos[i]);
    blocos[i] = NULL;
  }

  printf("\nMemoria depois:\n");
  for (int i = 0; i < NUM_BLOCOS; i++) {
    if (blocos[i] != NULL) {
      printf("%2d - %4zu bytes - %p\n", i, TAMANHOS[i], blocos[i]);
    } else {
      printf("%2d - %4zu bytes - NÃO ALOCADA\n", i, TAMANHOS[i]);
    }
  }

  printf("\n");
  mymemory_display(memory);
  printf("\n");
}

int main(void) {

  memory = mymemory_init(TAMANHOS[27]);

  if (memory == NULL) {
    printf("Init falhou\n");
    return 0;
  }
  teste_alloc_basico();
  teste_cleanup();
  mymemory_cleanup(memory);
  memory = NULL;
}
