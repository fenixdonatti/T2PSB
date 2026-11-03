/*
    main.c - Programa de testes do alocador de memoria

    Este arquivo não implementa o alocador: ele apenas usa as funções
    declaradas em mymemory.h para verificar se elas funcionam.

    Uso de IA:
        - Foi utilizado o Claude para gerar os valores do 'tamanhos'
        - Foi utilizado o Claude para fazer o algoritmo de embaralhamento dos indices do array 'tamanhos'
*/
#include "mymemory.c"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static mymemory_t *memory = NULL;
static const size_t tamanhos[50] = {
    10, 25, 50, 100, 250, 500, 1000, 15, 30, 60,
    120, 300, 600, 12, 40, 80, 150, 350, 700, 20,
    35, 70, 140, 280, 560, 18, 45, 90, 180, 360,
    720, 22, 55, 110, 220, 440, 880, 14, 28, 56,
    112, 224, 448, 896, 11, 33, 66, 132, 264, 528
};


void limparTerminal() {
  #if defined(_WIN32) || defined(_WIN64)
    system("cls");
  #else
    system("clear");
  #endif
  }

void memory_init() {
  limparTerminal();

  int pool_size;
  printf("Digite o tamanho do pool de memória: ");

  if (scanf("%d", &pool_size) != 1 || pool_size <= 0) {
    printf("Entrada inválida. Por favor, digite um número inteiro positivo.\n");
    exit(1);
  }

  memory = mymemory_init(pool_size);
  if (memory == NULL) {
    printf("Erro ao inicializar o pool de memória.\n");
    exit(1);
  }
}

void memory_aloc() {
  limparTerminal();

  size_t block_size;
  printf("Digite o tamanho do bloco a ser alocado: ");

  if (scanf("%zu", &block_size) != 1 || block_size <= 0) {
    printf("Entrada inválida. Por favor, digite um número inteiro positivo.\n");
    return;
  }

  void *ptr = mymemory_alloc(memory, block_size);
  if (ptr == NULL) {
    printf("Falha na alocação de memória.\n");
  } else {
    printf("Bloco de %zu bytes alocado em %p.\n", block_size, ptr);
  }
}

void memory_remove() {
  limparTerminal();

  void *ptr;

  printf("Digite o endereço do bloco a ser liberado (em hexadecimal, ex: "
         "0x7ffee3bff6c0): ");
  if (scanf("%p", &ptr) != 1) {
    printf("Entrada inválida. Por favor, digite um endereço válido.\n");
    return;
  }

  mymemory_free(memory, ptr);
}

void memory_display() {
  limparTerminal();

  if (memory == NULL) {
    printf("O pool de memória não foi inicializado. Inicialize primeiro.\n");
    return;
  }
  mymemory_display(memory);
}

void memory_stats() {
  limparTerminal();

  if (memory == NULL) {
    printf("O pool de memória não foi inicializado. Inicialize primeiro.\n");
    return;
  }
  mymemory_stats(memory);
}

void memory_cleanup() {
  limparTerminal();

  if (memory != NULL) {
    mymemory_cleanup(memory);
    memory = NULL;

    printf("Pool de memória limpo.\n");
  }
}

void test_memory() {
  limparTerminal();

  int n = sizeof(tamanhos) / sizeof(tamanhos[0]);
  int ordem[50];

  for (int i = 0; i < n; i++) ordem[i] = i;

  /* Fisher-Yates: embaralha os indices */
  srand((unsigned)time(NULL));
  for (int i = n - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    int tmp = ordem[i];
    ordem[i] = ordem[j];
    ordem[j] = tmp;
  }

  for (int i = 0; i < n; i++) {
    int k = ordem[i];
    void *ptr = mymemory_alloc(memory, tamanhos[k]);

    if (ptr == NULL) {
      printf("Falha na alocação de %zu bytes.\n", tamanhos[k]);
    } else {
      printf("Bloco de %zu bytes alocado em %p.\n", tamanhos[k], ptr);
    }
  }
}

int main() {
  do {
    printf("\nMenu:\n");

    printf("Escolha uma opção: \n");
    if (memory) {
      printf("2. Alocar bloco de memória\n");
      printf("3. Liberar bloco de memória\n");
      printf("4. Exibir estatísticas da memória\n");
      printf("5. Exibir blocos alocados\n");
      printf("6. Testar memória com blocos predefinidos\n");
      printf("7. Limpar pool de memória\n");
    } else {
      printf("1. Inicializar pool de memória\n");
    }
    printf("0. Sair\n");

    int choice;
    if (scanf("%d", &choice) != 1) {
      printf("Entrada inválida. Por favor, digite um número inteiro.\n");
      // Limpar o buffer de entrada
      while (getchar() != '\n')
        ;
      continue;
    }

    switch (choice) {
    case 1:
      memory_init();
      break;
    case 2:
      if (memory == NULL) {
        printf(
            "O pool de memória não foi inicializado. Inicialize primeiro.\n");
      } else {
        memory_aloc();
      }
      break;
    case 3:
      if (memory == NULL) {
        printf(
            "O pool de memória não foi inicializado. Inicialize primeiro.\n");
      } else {
        memory_remove();
      }
      break;
    case 4:
      if (memory == NULL) {
        printf(
            "O pool de memória não foi inicializado. Inicialize primeiro.\n");
      } else {
        memory_stats();
      }
      break;
    case 5:
      if (memory == NULL) {
        printf(
            "O pool de memória não foi inicializado. Inicialize primeiro.\n");
      } else {
        memory_display();
      }
      break;
    case 6:
      if (memory == NULL) {
        printf(
            "O pool de memória não foi inicializado. Inicialize primeiro.\n");
      } else {
        test_memory();
      }
      break;
    case 7:
      if (memory == NULL) {
        printf(
            "O pool de memória não foi inicializado. Inicialize primeiro.\n");
      } else {
        memory_cleanup();
      }
      break;
    case 0:
      limparTerminal();
      printf("Saindo do programa.\n");
      if (memory != NULL) {
        mymemory_cleanup(memory);
        memory = NULL;
      }
      exit(0);
      break;
    default:
      printf("Opção inválida. Tente novamente.\n");
    }
  } while (1);
}
