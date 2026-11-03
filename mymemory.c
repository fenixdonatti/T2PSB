/*
  mymemory.c - Implementacao do alocador de memoria
 
  Ideia geral:
    - O pool é um unico bloco obtido com malloc em mymemory_init.
    - A lista encadeada guarda SOMENTE os blocos ocupados, sempre
      ordenada pelo endereco de inicio (do menor para o maior).
    - O espaco livre não é guardado: ele e calculado como as lacunas
      entre blocos consecutivos da lista.
 
  Uso de IA:
    - Foi utilizado o ClaudeCode para:
        Estudar sobre o alocação de memória em C
        Arrumar espaçamentos
        Melhorar o nome das variáveis
        Auxilio na função 'mymemory_stats' e 'mymemory_display'
*/


#include <stdio.h>
#include <stdlib.h>
#include "mymemory.h"


static size_t offset_of(mymemory_t *memory, void *ptr)
{
    char *pool_start = (char *)memory->pool;
    char *address = (char *)ptr;
    return (size_t)(address - pool_start);
}

mymemory_t *mymemory_init(size_t size)
{
    if (size == 0) return NULL;

    mymemory_t *memory;
    memory = malloc(sizeof(mymemory_t));
    if (memory == NULL) return NULL;

    memory->pool = malloc(size);
    if (memory->pool == NULL) {
        free(memory);
        return NULL;
    }

    memory->total_size = size;
    memory->head = NULL;
    return memory;
}

void *mymemory_alloc(mymemory_t *memory, size_t node_size)
{
    allocation_t *previous = NULL;          /* nó anterior ao 'current' no percurso */
    allocation_t *current = NULL;           /* nó que está sendo visitado */

    size_t gap_start = 0;                   /* offset onde a lacuna atual começa (0 = início do pool) */
    size_t gap_size = 0;

    int found = 0;                          /* 1 se já achou alguma lacuna que serve */
    allocation_t *best_previous = NULL;     /* nó depois do qual o novo bloco será inserido (NULL = cabeça) */
    size_t best_start = 0;                  /* offset de início da melhor lacuna */
    size_t best_size = 0;

    allocation_t *new_node;

    if (memory == NULL || node_size == 0 || node_size > memory->total_size) return NULL;

    /* 1: Olhar a lacuna que existe antes de cada bloco */
    current = memory->head;
    while (current != NULL) {
        /* lacuna = início do bloco atual - fim do bloco anterior (ou início do pool) */
        gap_size = offset_of(memory, current->start) - gap_start;

        if (gap_size >= node_size) {
            /* Best Fit: aceita se for a primeira ou menor que a melhor até agora */
            if (found == 0 || gap_size < best_size) {
                found = 1;                  /* marca que achou uma lacuna */
                best_previous = previous;   /* guarda onde inserir na lista */
                best_start = gap_start;     /* guarda onde o bloco vai começar */
                best_size = gap_size;       /* guarda o tamanho para comparar depois */
            }
        }

        /* A próxima lacuna começa onde este bloco termina */
        gap_start = offset_of(memory, current->start) + current->size;
        previous = current;                 /* avança: o atual vira o anterior */
        current = current->next;            /* vai para o próximo bloco */
    }

    /* 2: Olhar a lacuna final, entre o último bloco e o fim do pool
        Aqui 'previous' é o último nó da lista */

    gap_size = memory->total_size - gap_start;      /* do fim do último bloco até o fim do pool */
    if (gap_size >= node_size) {
        if (found == 0 || gap_size < best_size) {   /* mesma regra do Best Fit */
            found = 1;
            best_previous = previous;       /* inserir depois do último nó (NULL se lista vazia) */
            best_start = gap_start;
            best_size = gap_size;
        }
    }

    // Nenhuma lacuna é grande o suficiente
    if (found == 0) return NULL;

    /* 3: Criar o nó */
    new_node = malloc(sizeof(allocation_t));
    if (new_node == NULL) return NULL;

    /* endereço real = início do pool + offset (cast p/ char* para somar em bytes) */
    new_node->start = (char *)memory->pool + best_start;
    new_node->size = node_size; 

    /* 4: Inserir na lista */
    if (best_previous == NULL) {                    /* lacuna antes do 1º bloco, ou lista vazia */
        new_node->next = memory->head;              /* novo nó aponta para a antiga cabeça */
        memory->head = new_node;                    /* novo nó vira a cabeça */
    } else {                                        /* lacuna depois de best_previous */
        new_node->next = best_previous->next;       /* novo nó aponta para quem vinha depois */
        best_previous->next = new_node;             /* anterior passa a apontar para o novo nó */
    }

    return new_node->start;
}

void mymemory_free(mymemory_t *memory, void *ptr)
{
    allocation_t *previous = NULL;
    allocation_t *current;

    if (memory == NULL || ptr == NULL) return;

    /* Procura o nó cujo 'start' é igual ao ptr */
    current = memory->head;
    while (current != NULL && current->start != ptr) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) return;

    /* Tira o nó da lista */
    if (previous == NULL) {
        memory->head = current->next;       // Era o primeiro
    } else {
        previous->next = current->next;     // Estava no meio ou no fim
    }

    free(current);
}

void mymemory_display(mymemory_t *memory)
{
    allocation_t *current;
    
    if (memory == NULL) return; 

    printf("Display do pool de %zu bytes: \n", memory->total_size);
    printf("-----------------------------------------------\n");
    printf("|     Inicio     |      Fim       |  Tamanho  |\n");


    current = memory->head;
    while (current != NULL) {
        printf("| %14p | %14p | %9zu |\n",
            current->start,                                         /* endereço inicial */
            (void *)((char *)current->start + current->size - 1),   /* endereço do último byte (char* soma em bytes) */
            current->size);                                         /* tamanho em bytes */
        current = current->next;
    }
    printf("-----------------------------------------------\n");
}

void mymemory_stats(mymemory_t *memory)
{
    allocation_t *current;
    size_t num_allocation = 0;      /* quantidade de blocos alocados */
    size_t total_allocated = 0;     /* soma dos tamanhos dos blocos, em bytes */
    size_t biggest_free = 0;        /* maior lacuna livre encontrada */
    size_t fragments = 0;           /* quantidade de lacunas livres */
    
    size_t gap_start = 0;           /* offset onde a lacuna atual começa (0 = início do pool) */
    size_t gap_size;

    if (memory == NULL) return;

    /* Mesma ideia do alloc: a lacuna antes de cada bloco */
    current = memory->head;
    while (current != NULL) {
        /* lacuna = início do bloco atual - fim do bloco anterior (ou início do pool) */
        gap_size = offset_of(memory, current->start) - gap_start;

        if (gap_size > 0) {
            fragments++;                    /* mais um fragmento livre */
            if (gap_size > biggest_free) {  /* atualiza o maior bloco livre */
                biggest_free = gap_size;
            }
        }

        num_allocation++;
        total_allocated = total_allocated + current->size;  /* soma o tamanho dele */
        
        /* a próxima lacuna começa onde este bloco termina */
        gap_start = offset_of(memory, current->start) + current->size;
        current = current->next;
    }

    /* Lacuna final, entre o ultimo bloco e o fim do pool */
    gap_size = memory->total_size - gap_start;
    if (gap_size > 0) {                 /* sobrou espaço no fim do pool */
        fragments++;
        if (gap_size > biggest_free) {
            biggest_free = gap_size;
        }
    }
    
    printf("Estatisticas:\n");
    printf("------------------------------------------\n");
    printf("| Numero de alocacoes: %zu                 |\n", num_allocation);
    printf("| Memoria alocada:     %zu bytes           |\n", total_allocated);
    printf("| Memoria livre:       %zu bytes           |\n", memory->total_size - total_allocated);
    printf("| Maior bloco livre:   %zu bytes           |\n", biggest_free);
    printf("| Fragmentos livres:   %zu                 |\n", fragments);
    printf("------------------------------------------\n");
}

void mymemory_cleanup(mymemory_t *memory)
{
    allocation_t *current;
    allocation_t *next;
 
    if (memory == NULL) return;
 
    current = memory->head;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
 
    free(memory->pool);     /* libera o pool */
    free(memory);           /* libera a estrutura de controle */
}
