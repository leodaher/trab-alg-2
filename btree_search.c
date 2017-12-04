#include <stdio.h>
#include <stdlib.h>
#include "btree_search.h"
#include "btree.h"

int searchRec(bTree *bt, pagina *p, chave *c, char* filename) {
	int i = 0 ;

	while (i < p->cntChave && p->chaves[i].id < c->id) {
		i++ ;
	}

	if (p->chaves[i].id == c->id) {
		c->offset = p->chaves[i].offset ;
		return 1 ;
	} else if (p->folha == '1') {
		return 0 ;
	} else {
		loadPage(bt, p, p->filhos[i], filename) ;
		return searchRec(bt, p, c, filename) ;
	}
}

long search(bTree *bt, int id, char* filename) {
	if (bt->raiz == -1) {
		return -1 ;
	}

	pagina p ;
	chave c ;
	c.id = id ;

	loadPage(bt, &p, bt->raiz, filename) ;

	if (!searchRec(bt, &p, &c, filename)) {
		freePage(&p) ;
		return -1 ;
	}


	return c.offset ;

}
