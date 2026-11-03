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
    allocation_t *previous = NULL;
    allocation_t *current;
    size_t gap_start = 0;
    size_t gap_size;

    int found = 0;
    allocation_t *best_previous = NULL;
    size_t best_start = 0;
    size_t best_size = 0;

    allocation_t *new_node;

    if (memory == NULL || node_size == 0 || node_size > memory->total_size) return NULL;

    /* 1: Olhar a lacuna que existe antes de cada bloco */
    current = memory->head;
    while (current != NULL) {
        gap_size = offset_of(memory, current->start) - gap_start;

        if (gap_size >= node_size) {
            if (found == 0 || gap_size < best_size) {
                found = 1;
                best_previous = previous;
                best_start = gap_start;
                best_size = gap_size;
            }
        }

        /* A próxima lacuna começa onde este bloco termina */
        gap_start = offset_of(memory, current->start) + current->size;
        previous = current;
        current = current->next;
    }

    /* 2: Olhar a lacuna final, entre o último bloco e o fim do pool
        Aqui 'previous' é o último nó da lista */

    gap_size = memory->total_size - gap_start;
    if (gap_size >= node_size) {
        if (found == 0 || gap_size < best_size) {
            found = 1;
            best_previous = previous;
            best_start = gap_start;
            best_size = gap_size;
        }
    }

    // Nenhuma lacuna é grande o suficiente
    if (found == 0) return NULL;

    /* 3: Criar o nó */
    new_node = malloc(sizeof(allocation_t));
    if (new_node == NULL) return NULL;

    new_node->start = (char *)memory->pool + best_start;
    new_node->size = node_size;

    /* 4: Inserir na lista */
    if (best_previous == NULL) {
        new_node->next = memory->head;
        memory->head = new_node;
    } else {
        new_node->next = best_previous->next;
        best_previous->next = new_node;
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

    /* Sem coluna de estado, so os blocos alocados sao listados */
    current = memory->head;
    while (current != NULL) {
        printf("| %14p | %14p | %9zu |\n",
            current->start,
            (void *)((char *)current->start + current->size - 1),
            current->size);
        current = current->next;
    }
    printf("-----------------------------------------------\n");
}

void mymemory_stats(mymemory_t *memory)
{
    allocation_t *current;
    size_t num_allocation = 0;
    size_t total_allocated = 0;
    size_t biggest_free = 0;
    size_t fragments = 0;
    size_t gap_start = 0;
    size_t gap_size;
 
    if (memory == NULL) return;
 
    /* Mesma ideia do alloc: a lacuna antes de cada bloco */
    current = memory->head;
    while (current != NULL) {
        gap_size = offset_of(memory, current->start) - gap_start;
 
        if (gap_size > 0) {
            fragments++;
            if (gap_size > biggest_free) {
                biggest_free = gap_size;
            }
        }
 
        num_allocation++;
        total_allocated = total_allocated + current->size;
        gap_start = offset_of(memory, current->start) + current->size;
        current = current->next;
    }
 
    /* Lacuna final, entre o ultimo bloco e o fim do pool */
    gap_size = memory->total_size - gap_start;
    if (gap_size > 0) {
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
