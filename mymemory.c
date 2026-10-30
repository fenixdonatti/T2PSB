/*
  mymemory.c - Implementacao do alocador de memoria
 
  Ideia geral:
    - O pool é um unico bloco obtido com malloc em mymemory_init.
    - A lista encadeada guarda SOMENTE os blocos ocupados, sempre
      ordenada pelo endereco de inicio (do menor para o maior).
    - O espaco livre não é guardado: ele e calculado como as lacunas
      entre blocos consecutivos da lista.
 
  Uso de IA:
    - Descrever aqui se vocês usarem ia em algo
*/


#include <stdio.h>
#include <stdlib.h>
#include "mymemory.h"


/*
    Aloca o bloco de memória total e retorna um ponteiro para ele.
*/
mymemory_t *mymemory_init(size_t size)
{
    if (size == 0) return NULL;

    // Reserva o espaço na memória da lista encadeada fora do pool
    mymemory_t *memory = malloc(sizeof(mymemory_t));
    if (memory == NULL) return NULL;

    // Cria o pool e guarda o endereço do primeiro byte
    memory->pool = malloc(size);
    if (memory->pool == NULL) {
        free(memory);           // Se o pool falhar, libera a struct antes de sair
        return NULL;
    }

    memory->total_size = size;      // Guarda o tamanho do pool
    memory->head = NULL;            // Marca a lista vazia
    return memory;                  // Devolve o ponteiro para a struct
}


/*
    Tenta alocar um bloco de memória de tamanho especificado.

    Se for bem-sucedida, retorna um ponteiro para o ínicio do bloco.
    Se for mal-sucedida, retorna NULL.
*/
void *mymemory_alloc(mymemory_t *memory, size_t size)
{
    if (memory == NULL || size == 0) return NULL;

    char *pool_inicio = (char *)memory->pool;
    char *pool_fim = pool_inicio + memory->total_size;

    char *melhor_inicio = NULL;                 // Ainda não achei a lacuna
    size_t melhor_tamanho = 0;                  // É o tamanho da melhor lacuna até agora
    allocation_t *melhor_anterior = NULL;       // É o nó depois do qual vai inserir

    /*
        Essa lacuna vai do começo do pool até o primeiro bloco.
        Se a lista está vazia (head == NULL), ela vai até o fim do pool.
    */
    char *fim = (memory->head != NULL)          // É onde essa lacuna termina
        ? (char *)memory->head->start 
        : pool_fim;
    size_t tamanho = fim - pool_inicio;
    if (tamanho >= size) {                      // Se cabe, ela já vira a melhor.
        melhor_inicio = pool_inicio;
        melhor_tamanho = tamanho;
    }

    /*
        O for percorre a lista nó por nó.

        O 'inicio' é onde o bloco 'a' termina, ou seja, onde a lacuna começa.
        O 'proximo' é onde a lacuna termina.
        A 'lacuna' é a diferença em bytes entre os dois.

        O if é o Best Fit
    */
    for (allocation_t *a = memory->head; a != NULL; a = a->next) {      
        char *inicio = (char *)a->start + a->size;
        char *proximo = (a->next != NULL) 
            ? (char *)a->next->start 
            : pool_fim;
        size_t lacuna = proximo - inicio;

        if (lacuna >= size && (melhor_inicio == NULL || lacuna < melhor_tamanho)) {
            melhor_inicio = inicio;
            melhor_tamanho = lacuna;
            melhor_anterior = a;
        }
    }

    // Se nenhuma lacuna tinha espaço, retorna NULL
    if (melhor_inicio == NULL) return NULL;

    /*
        Criar e inserir o nó
    */
    allocation_t *novo = malloc(sizeof(allocation_t));
    if (novo == NULL) return NULL;
    novo->start = melhor_inicio;
    novo->size = size;

    if (melhor_anterior == NULL) {
        novo->next = memory->head;
        memory->head = novo;
    } else {
        novo->next = melhor_anterior->next;
        melhor_anterior->next = novo;
    }

    return novo->start;
}