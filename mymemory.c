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
        Criação da função 'offset_of'
        Corrigir erros em comentarios feitos para estudo para apresentação
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
    if (size == 0) return NULL; // size 0 nao faz sentido pq nao cabe nada

    mymemory_t *memory;     // declara o ponteiro sem endereço válido ainda (tem lixo)
    memory = malloc(sizeof(mymemory_t));    // aqui a struct começa a existir. reserva no heap espaço para a struct: 2 ponteiros (pool, head) + 1 size_t (total_size) -> 24 bytes em 64 bits
    if (memory == NULL) return NULL; //se nao tem memoria pra alocar retorna null

    memory->pool = malloc(size); // acessa o ponteiro de memory e vai até pool, guardando o endereço de um bloco de size bytes
    if (memory->pool == NULL) { //se falha libera a struct ja alocada, senao ela vazaria (memory leak)
        free(memory);
        return NULL;
    }
    //preenche os dois outros campos da estrutura
    memory->total_size = size; // guarda o tamanho do pool (o ponteiro não sabe o tamanho)
    memory->head = NULL;    // lista de alocacoes vazia, pra nao percorrer uma lista que nao existe ainda. malloc não zera a memória, então head teria lixo
    return memory;  // retorna o endereço em que o struct de controle fica (o pool esta em memory->pool)
}

void *mymemory_alloc(mymemory_t *memory, size_t node_size)
{
    allocation_t *previous = NULL;  // nó anterior ao que to agora, começa sem nenhum
    allocation_t *current = NULL;   // nó atual
    size_t gap_start = 0;    // onde começa o espaço livre atual; no inicio, o começo do pool (0)
    size_t gap_size = 0;    // tamanho da lacuna atual

    int found = 0;  // nao achou lacuna que serve ainda
    allocation_t *best_previous = NULL; // depois de qual nó o novo entra (NULL = vira a cabeça)
    size_t best_start = 0;  // onde começa a melhor lacuna
    size_t best_size = 0;   // tamanho da melhor lacuna

    allocation_t *new_node; // nó que vamos criar (ponteiro com endereço aleatorio ainda)

    if (memory == NULL || node_size == 0 || node_size > memory->total_size) return NULL;
        // gerenciador não existe, o pedido é de 0 bytes ou o pedido é maior que o pool inteiro.
        //retornam null pq nao tem como fazer nada

    /* 1: Olhar a lacuna que existe antes de cada bloco */
    current = memory->head;     // começa no primeiro nó
    while (current != NULL) {   // while que roda a lista. se head for null nem roda
        gap_size = offset_of(memory, current->start) - gap_start;   // current start é ponteiro pro inicio do bloco ocupado.
            //offset ve quao longe do começo ele fica (em numero)
            // - gap_start retorna o tamanho entre o começo da lacuna e o começo do bloco que temos o endereco de inicio

        if (gap_size >= node_size) { // ve se cabe ou nao
            if (found == 0 || gap_size < best_size) { // primeira que cabe (nao tem com o que comparar) ou o que cabe melhor?
                found = 1;  // marca que ja achou uma lacuna que serve
                best_previous = previous;   // nó antes da lacuna: o novo vai entrar depois dele (NULL = antes do primeiro)
                best_start = gap_start;     // inicio da lacuna
                best_size = gap_size;
            }
        }

        /* A próxima lacuna começa onde este bloco termina */
        gap_start = offset_of(memory, current->start) + current->size; // a proxima começa onde o atual termina
        previous = current;     // o atual vira o anterior
        current = current->next;  // o proximo vira o atual
    }

    /* 2: Olhar a lacuna final, entre o último bloco e o fim do pool
        Aqui 'previous' é o último nó da lista */

    gap_size = memory->total_size - gap_start; //mede a lacuna entre o ultimo bloco e o final do pool. gap start é o final do ultimo bloco
    if (gap_size >= node_size) { // if igual ao da p1 pra checar se é o melhor caso
        if (found == 0 || gap_size < best_size) {
            found = 1;
            best_previous = previous;
            best_start = gap_start;
            best_size = gap_size;
        }
    }

    // Nenhuma lacuna é grande o suficiente
    if (found == 0) return NULL; // se nenhum dos gaps é grande o suficiente retorna que nao coube

    /* 3: Criar o nó */
    new_node = malloc(sizeof(allocation_t)); // nó de controle entra no heap
    if (new_node == NULL) return NULL;  // sem memoria para o nó: falha

    new_node->start = (char *)memory->pool + best_start; // converte o ponteiro para um de char (1 byte) e avança o tamanho de bytes do best_start
        // fica endereço + x bytes = endereço do inicio do bloco dentro do pool
    new_node->size = node_size;

    /* 4: Inserir na lista */
    if (best_previous == NULL) {  // se ta antes do primeiro bloco ou a lista ta vazia
        new_node->next = memory->head; // novo nó vira o head e aponta para o antigo primeiro
        memory->head = new_node;
    } else {
        new_node->next = best_previous->next; // se o gap ta depois de um nó existente ele entra entre best_previous e o nó que ficava depois dele
        best_previous->next = new_node;       // a ordem dessas duas linhas importa: invertidas, o resto da lista se perderia
    }

    return new_node->start; // devolve o endereço do bloco reservado no pool (nao o endereço do nó)
}

void mymemory_free(mymemory_t *memory, void *ptr)
{
    allocation_t *previous = NULL; //nó anterior ao atual (começa NULL)
    allocation_t *current; // nó que estamos olhando

    if (memory == NULL || ptr == NULL) return; // nao existe gerenciador ou o user mandou null (nao tem o que liberar)

    /* Procura o nó cujo 'start' é igual ao ptr */
    current = memory->head; // começa no primeiro nó
    while (current != NULL && current->start != ptr) { // anda ate achar o nó com start == ptr ou ate acabar a lista (current != NULL vem antes pra nao acessar NULL->start)
        previous = current; // anda na lista e atualiza o previous e current
        current = current->next;
    }

    if (current == NULL) return; // se nao achou é uma alocacao invalida, e nao faz nada igual o enunciado fala

    /* Tira o nó da lista */
    if (previous == NULL) {
        memory->head = current->next;       // Era o primeiro: head pula para o segundo nó
    } else {
        previous->next = current->next;     // Estava no meio ou no fim: anterior pula o nó removido
    }

    free(current); // devolve o nó de controle criado com o malloc (só depois de tirar o nodo da lista)
                   // o bloco no pool NAO recebe free: ele nao veio de malloc, so vira lacuna livre
}

void mymemory_display(mymemory_t *memory)
{
    allocation_t *current; // nó que estamos olhando

    if (memory == NULL) return; // sem gerenciador, nao tem o que mostrar

    printf("Display do pool de %zu bytes: \n", memory->total_size); // tamanho do pool (size_t = %zu)
    printf("-----------------------------------------------\n");
    printf("|     Inicio     |      Fim       |  Tamanho  |\n");


    current = memory->head; // começa no primeiro nó
    while (current != NULL) { //comeca no head ate null e mostra a lista
        printf("| %14p | %14p | %9zu |\n",
            current->start,                                         /* endereço inicial */
            (void *)((char *)current->start + current->size - 1),   /* endereço do último byte (char* soma em bytes) */
            current->size);                                         /* tamanho em bytes */
        current = current->next; // anda para o proximo nó
    }
    printf("-----------------------------------------------\n");
}

void mymemory_stats(mymemory_t *memory)
{
    allocation_t *current;              // nó que estamos olhando agora
    size_t num_allocation = 0;          // numero de blocos ocupados
    size_t total_allocated = 0;         // soma do tamanho dos blocos ocupados
    size_t biggest_free = 0;            // maior gap livre até o momento
    size_t fragments = 0;               // numero de gaps com tamanho maior que 0

    size_t gap_start = 0;               // inicio do gap livre atual
    size_t gap_size;                    // tamanho do gap atual

    if (memory == NULL) return; // sem gerenciador, nao tem o que calcular

    /* Mesma ideia do alloc: a lacuna antes de cada bloco */
    current = memory->head; // começa no primeiro nó
    while (current != NULL) {
        gap_size = offset_of(memory, current->start) - gap_start; // lacuna antes deste bloco = inicio dele - fim do anterior

        if (gap_size > 0) { // dois blocos colados -> lacuna de 0 bytes -> 0 fragmentos. logo só entra se o gap for maior que 0
            fragments++; // adiciona o gap na conta
            if (gap_size > biggest_free) {
                biggest_free = gap_size;  // atualiza o maior gap se for o caso
            }
        }

        num_allocation++; // aumenta o numero de alocacoes
        total_allocated = total_allocated + current->size; //adiciona o tamanho das alocacoes ao total

        gap_start = offset_of(memory, current->start) + current->size; // ajusta o inicio do prox gap
        current = current->next; // anda para o proximo nó
    }

    /* Lacuna final, entre o ultimo bloco e o fim do pool */
    gap_size = memory->total_size - gap_start; //tamanho do final do ultimo bloco até o final do pool
    if (gap_size > 0) {
        fragments++; // adiciona ao frag
        if (gap_size > biggest_free) { //atualiza caso seja maior que os gaps anteriores
            biggest_free = gap_size;
        }
    }

    //prints (memoria livre = total - alocado, que da o mesmo que somar as lacunas)
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
    allocation_t *current; // nó que estamos liberando
    allocation_t *next; // proximo nó (variavel local, guardado antes de liberar o atual)

    if (memory == NULL) return; // sem gerenciador, nao tem o que liberar

    /* Libera cada no, guardando o proximo ANTES do free */
    current = memory->head;
    while (current != NULL) {
        next = current->next;   // guarda o proximo enquanto o nó ainda é valido
        free(current);          // depois disso, ler current->next seria use after free
        current = next;         // anda usando o endereço guardado
    }

    // nao precisa atualizar o head nem religar a lista: a struct tambem vai ser liberada, entao ninguem le esses ponteiros de novo

    free(memory->pool);     /* libera o pool (antes da struct: memory->pool é lido atraves dela) */
    free(memory);           /* libera a estrutura de controle */
}
