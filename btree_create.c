#include <stdio.h>
#include <stdlib.h>
#include "btree.h"

int startBTree(bTree *bt, char* filename) {

	bt->bTFile = fopen(filename, "r") ;
	bt->ordem = ORDEM ;

	if (!bt->bTFile) {
		bt->raiz = -1 ;
		bt->nextRrn = 0 ;
		bt->bTFile = fopen(filename, "w") ;
		fclose(bt->bTFile) ;
		return 0 ;
	}

	fseek(bt->bTFile, 0, SEEK_END) ;

	if (ftell(bt->bTFile) == 0) {
		bt->raiz = -1 ;
		bt->nextRrn = 0 ;
		fclose(bt->bTFile) ;

		return 0 ;
	}

	rewind(bt->bTFile) ;
	fscanf(bt->bTFile, "%8d%8d", &bt->raiz, &bt->nextRrn) ;
	fclose(bt->bTFile) ;

	return 1 ;
}

void startPage(bTree *bt, pagina *p) {
	p->cntChave = 0 ;
	p->chaves = (chave *) malloc(sizeof(chave) * (bt->ordem - 1)) ;
	p->filhos = (int *) malloc(sizeof(int) * bt->ordem) ;
}


int rrnToOffset(int rrn) {
	return 2 * INT_TAM + rrn * PAGE_TAM ;
}


int loadPage(bTree *bt, pagina *p, int rrn, char* filename) {
	bt->bTFile = fopen(filename, "r") ;
	if (!bt->bTFile || bt->raiz == -1) {
		p->cntChave = 0 ;
		return 0 ;
	}

	startPage(bt, p) ;
	char *buffer = (char *) malloc(sizeof(char) * (PAGE_TAM + 1)) ;

	fseek(bt->bTFile, rrnToOffset(rrn), SEEK_SET) ;
	fgets(buffer, PAGE_TAM + 1, bt->bTFile) ;
	fclose(bt->bTFile) ;

	char *pos = buffer ;
	sscanf(pos, "%c%8d", &p->folha, &p->cntChave) ;
	pos += INT_TAM + CHAR_TAM ;

	int i ;
	for (i = 0; i < p->cntChave; i++) {
		sscanf(pos, "%8d%15ld", &p->chaves[i].id, &p->chaves[i].offset) ;
		pos += INT_TAM + LONG_TAM;
	}

	if (p->folha == '0') {
		for (i = 0; i <= p->cntChave; i++) {
			sscanf(pos, "%8d", &p->filhos[i]) ;
			pos += INT_TAM ;
		}
	}
	free(buffer) ;
  p->rrn = rrn ;
	return 1 ;
}

void freePage(pagina *p) {
	free(p->chaves) ;
	if (p->folha == '0') {
		free(p->filhos) ;
	}
}
