# T2PSB — Gerenciador de Memória

Trabalho 2 da disciplina **Programação de Software Básico**.

Biblioteca em C de gerenciamento de memória simples: aloca e libera blocos dentro de um *pool* pré-alocado. O pool é organizado por uma lista encadeada de alocações.

## Integrantes

Fênix Rodrigues Pedro
Francisco Schardosim Ferr
Davi Barcellos de Lucca

## Progresso

- [X]  `mymemory.h` (estruturas e protótipos)
- [X]  `mymemory_init`
- [X]  `mymemory_alloc` (Best Fit, inserção ordenada na lista)
- [X]  `mymemory_free`
- [X]  `mymemory_display`
- [X]  `mymemory_stats`
- [X]  `mymemory_cleanup`
- [X]  Vetor de 50 tamanhos em `main.c`
- [ ]  Testes em `main.c` (alocar, liberar, variar ordem, display, stats)
- [ ]  Compilar sem erros/avisos (`-Wall -Wextra`)
- [ ]  Verificar vazamentos com `valgrind`
- [ ]  Completar seção "Uso de IA"

## Arquivos


| Arquivo      | Descrição                                          |
| ------------ | ---------------------------------------------------- |
| `mymemory.h` | Estruturas de dados e protótipos das funções      |
| `mymemory.c` | Implementação do alocador                          |
| `main.c`     | Programa de testes (~50 blocos de tamanhos variados) |

## Estruturas de dados

```c
typedef struct allocation {
    void *start;              // início do bloco alocado
    size_t size;              // tamanho da alocação
    struct allocation *next;  // próxima alocação (lista encadeada)
} allocation_t;

typedef struct {
    void *pool;               // bloco de memória real
    size_t total_size;        // tamanho total do pool
    allocation_t *head;       // cabeça da lista de alocações
} mymemory_t;
```

A lista guarda **somente os blocos ocupados**, ordenada pelo endereço de início. O espaço livre não é armazenado: é calculado como as lacunas entre blocos consecutivos.

## Funções


| Função                                                | Descrição                                                                                    |
| ------------------------------------------------------- | ---------------------------------------------------------------------------------------------- |
| `mymemory_t* mymemory_init(size_t size)`                | Aloca o pool total e retorna o ponteiro para a estrutura                                       |
| `void* mymemory_alloc(mymemory_t *memory, size_t size)` | Aloca um bloco de`size` bytes. Retorna o início do bloco ou `NULL` se falhar                  |
| `void mymemory_free(mymemory_t *memory, void *ptr)`     | Libera a alocação apontada por`ptr`. Se `ptr` não for uma alocação válida, não faz nada |
| `void mymemory_display(mymemory_t *memory)`             | Exibe cada alocação atual (início e tamanho)                                                |
| `void mymemory_stats(mymemory_t *memory)`               | Exibe estatísticas (abaixo)                                                                   |
| `void mymemory_cleanup(mymemory_t *memory)`             | Libera todas as alocações e o pool                                                           |

### Estatísticas (`mymemory_stats`)

- Número total de alocações
- Memória total alocada (bytes)
- Memória total livre (bytes)
- Maior bloco contíguo livre
- Número de fragmentos livres (lacunas entre alocações)

## Estratégia de alocação

Estratégias possíveis (`mymemory_alloc` procura a lacuna conforme a escolhida):

- **First Fit** — primeira lacuna suficientemente grande
- **Best Fit** — menor lacuna suficientemente grande
- **Worst Fit** — maior lacuna

Estratégia adotada: **Best Fit**

## Restrições

`malloc()`/`free()` são usados **apenas** para o pool e para as estruturas de controle (`mymemory_t` e nós `allocation_t`).

## Compilação e execução

`main.c` inclui `mymemory.c` diretamente, então compile só `main.c`:

```sh
gcc -Wall -Wextra -o t2 main.c
./t2
```

O código deve compilar sem erros ou avisos. Para checar vazamentos:

```sh
valgrind --leak-check=full ./t2
```

## Testes

`main.c` aloca ~50 blocos de tamanhos variados (5 a 2048 bytes), em ordens diferentes, e verifica alocação, liberação, fragmentação e estatísticas.

## Uso de IA

- Claude gerou os valores do vetor de tamanhos em `main.c`.
- Claude gerou este README a partir do enunciado.
- _Descrever aqui qualquer outro uso de IA ou ajuda de colegas._
