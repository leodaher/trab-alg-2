/*  Trabalho 3 - Algoritimos e Estruturas de Dados II
 *
 *  Desenvolvedores:
 *    Guilherme Prearo
 *    Gustavo Nicolau Goncalves
 *    Pedro V. B. Jeronymo
 *
 *  (Compilador gcc 4.9.2)
 */

#ifndef BTREE_H_INCLUDED
#define BTREE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>


// ORDEM - Quantidade máxima de filhos em um nó
#define ORDEM 6

//Tamnho de uma variável do tipo "long" no arquivo
//Ex.: se l = 2003, no arquivo será "000000000002003"
#define LONG_SIZE 15

//Tamanho de uma variável do tipo "int" no arquivo
//Ex.: se i = 3000, no arquivo será "00003000"
#define INT_SIZE 8

//Tamanho de cada chave no arquivo
#define CHAVE_SIZE INT_SIZE + LONG_SIZE

//Tamanho de uma variável do tipo "char" no arquivo
 #define CHAR_SIZE 1

//Tamanho de cada página no arquivo
//Organização: "[FOLHA][QUANTIDADE DE CHAVES][ID1][OFFSET1]..[IDN][OFFSETN][FILHO1]..[FILHON+1]"
#define PAGE_SIZE (CHAR_SIZE + INT_SIZE + CHAVE_SIZE * (ORDEM - 1) + LONG_SIZE * ORDEM)

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

/*
	Nome: initBT

	Parâmetros:
		bt - ponteiro para bTree

	Função: Inicializa as variáveis da árvore

	Retorno:
		1 - Sucesso
		0 - Arquivo de indice não existe
*/
int initBT(bTree *bt, char* filename) ;


/*
	Nome: loadPage

	Parâmetros:
		bt - ponteiro para bTree
		p - ponteiro para página
		rrn - localização da página no arquivo

	Função: Lê os dados do arquivo na posição especificada pelo
	RRN e carrega para a memória principal no endereço apontado por p

	Retorno:
		1 - Sucesso
		0 - Arquivo de indice não existe
*/
int loadPage(bTree *bt, pagina *p, int rrn) ;

/*
	Nome: printPage

	Parâmetros:
		p - ponteiro para página

	Função: Imprime os dados da página p
*/
void printPage(pagina *p) ;

/*
	Nome: search

	Parâmetros:
		bt - ponteiro para bTree
		id - ID a ser pesquisado

	Função: Pesquisa o ID dado na árvore e retorna o
	offset no arquivo de dados do registro correspondente

	Retorno:
		>= 0 - Sucesso
		-1 - ID não encontrado
*/
long search(bTree *bt, int id, char* filename) ;

int insert(bTree *bt, int id, long offset, char* filename, FILE * flog) ;

#endif
