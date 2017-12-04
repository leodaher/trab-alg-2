#ifndef BTREE_H_INCLUDED
#define BTREE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

// Constantes importantes
#define ORDEM 6
#define LONG_TAM 15
#define INT_TAM 8
#define CHAVE_TAM INT_TAM + LONG_TAM
#define CHAR_TAM 1
#define PAGE_TAM (CHAR_TAM + INT_TAM + CHAVE_TAM * (ORDEM - 1) + LONG_TAM * ORDEM)

// Nome do arquivo de índices
#define BTFILE "arvore.idx"

typedef long offset_t ;

typedef struct {
	int id;
	offset_t offset;
} chave;

typedef struct {
  int rrn ;
	int cntChave ;
	chave *chaves ;
	int *filhos ;
	char folha;
} pagina;

typedef struct {
	int raiz ;
	int nextRrn ;
	int ordem ;
	FILE *bTFile ;
} bTree;

#endif
