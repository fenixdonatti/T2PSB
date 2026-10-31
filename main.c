/*
    main.c - Programa de testes do alocador de memoria

    Este arquivo não implementa o alocador: ele apenas usa as funções
    declaradas em mymemory.h para verificar se elas funcionam.

    Uso de IA:
        - Foi utilizado o Claude para gerar os valores do size_t.
*/
#ifdef __MINGW32__
#define __USE_MINGW_ANSI_STDIO 1   /* faz o printf do MinGW aceitar %zu */
#endif
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


static mymemory_t *memory = NULL;
static void *blocos[NUM_BLOCOS];
static int falhas = 0;

// Contador de falhas

void teste_alloc_basico(void){
	  //void *a;
	  
	  int acertos = 0;
	//int a_tamanho = 1;
	//int b_tamanho = 1;
	
	
	/* if (memory == NULL) {
        printf("init falhou\n");
        return;
    }*/ 
	
	//printf("digite valor2: \n");
	//scanf("%d", &b_tamanho)
	
	
	
	    for (int i = 0; i < NUM_BLOCOS; i++) {
        //a = mymemory_alloc(memory, TAMANHOS[i]);
		blocos[i] = mymemory_alloc(memory, TAMANHOS[i]);
		if (blocos[i]!= NULL){
		
		//printf("alloc de a funcionou\n");
		printf("colocado em %p \n\n", blocos[i] );
		acertos++;
		printf("%d ACERTOS \n", acertos);
	}else{
		
		//printf("alloc de a falhou\n");
		 falhas++;
		 printf("%d RECUSADAS \n\n ", falhas);
	}
	
	
	}
	mymemory_display(memory);
	 //mymemory_cleanup(memory);
		
		}


void teste_cleanup(){
	
	printf("\n\nmemoria antes:\n\n");
	
	mymemory_display(memory);
	printf("\n");
	 for (int i = 0; i < NUM_BLOCOS; i += 2) {
        
	printf("%2d LIBERADO %4zu bytes  %p\n", i, TAMANHOS[i], blocos[i]);
		mymemory_free(memory, blocos[i]);
        blocos[i] = NULL;
    }
	
	printf("\n\n\nmemoria depois:\n\n");
	
	mymemory_display(memory);
}

	

int main(void)
{
	
	 memory = mymemory_init(1000);
	 
	  if (memory == NULL) {
        printf("init falhou\n");
        return 0;
    }
		  teste_alloc_basico();
			teste_cleanup();
			mymemory_cleanup(memory);
     memory = NULL;
	 
	 
}







