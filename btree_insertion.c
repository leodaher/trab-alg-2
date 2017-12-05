#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"
#include "btree_search.h"
#include "btree_create.h"

int wrPage(bTree *bt, pagina *p, char* filename) {
    bt->bTFile = fopen(filename, "r+") ;

    if (!bt->bTFile) {
        return 0 ;
    }

    char *buf = (char *) malloc(sizeof(char) * PAGE_TAM) ;
    buf[0] = '\0' ;

    sprintf(buf, "%s%c%0*d", buf, p->folha, INT_TAM, p->cntChave) ;

    int i ;
    for (i = 0; i < p->cntChave; i++) {
        sprintf(buf, "%s%0*d%0*ld", buf, INT_TAM, p->chaves[i].id, LONG_TAM, p->chaves[i].offset) ;
    }

    if (p->folha == '0') {
        for (i = 0; i <= p->cntChave; i++) {
        	sprintf(buf, "%s%0*d", buf, INT_TAM, p->filhos[i]) ;
        }
    }

    fseek(bt->bTFile, rrnToOffset(p->rrn), SEEK_SET) ;
    fprintf(bt->bTFile, "%s%0*d", buf, (int) (PAGE_TAM - strlen(buf)), 0) ;
    fclose(bt->bTFile) ;
    free(buf) ;

    return 1 ;
}


int atuCab(bTree *bt, char* filename) {
	bt->bTFile = fopen(filename, "r+") ;

    if (!bt->bTFile) {
        return 0 ;
    }

    fprintf(bt->bTFile, "%0*d%0*d", INT_TAM, bt->raiz, INT_TAM, bt->nextRrn) ;

    fclose(bt->bTFile) ;
    return 1 ;
}

int insertRec(bTree *bt, pagina *p, chave *c, int *newChild, char* filename, FILE * flog) {
	int i, j ;

	i = 0 ;

	while (i < p->cntChave && p->chaves[i].id < c->id) {
		i++ ;
	}

	if (p->folha == '0') {
		pagina *child = malloc(sizeof(pagina));
		if (p->filhos[i] > bt->nextRrn) {
			printf("Errado\n");
		}
		loadPage(bt, child, p->filhos[i], filename) ;

		if (insertRec(bt, child, c, newChild, filename, flog) == 1) {
			freePage(child) ;
			free(child) ;
			return 1 ;
		}
		freePage(child) ;
		free(child) ;
	}

	if (p->cntChave < ORDEM - 1) {

		for (j = p->cntChave; j > i; j--) {
			p->chaves[j] = p->chaves[j - 1] ;
		}

		p->chaves[i] = *c ;

		if (p->folha == '0') {
			for (j = p->cntChave + 1; j > i + 1; j--) {
				p->filhos[j] = p->filhos[j - 1] ;
			}
			p->filhos[i + 1] = *newChild ;
		}

		p->cntChave++ ;
		wrPage(bt, p, filename) ;

		return 1 ;
	} else {

		fprintf(flog, "Divisao de no - pagina %d\n", p->rrn);

		pagina *right = (pagina *) malloc(sizeof(pagina)) ;
		startPage(bt, right) ;
		right->folha = p->folha ;
		right->rrn = bt->nextRrn++ ;
		atuCab(bt, filename) ;
		chave *pDividir = (chave *) malloc(sizeof(chave) * ORDEM) ;
		for (j = 0; j < i; j++) {
			pDividir[j] = p->chaves[j] ;
		}
		pDividir[i] = *c ;
		for (j = i; j < p->cntChave; j++) {
			pDividir[j + 1] = p->chaves[j] ;
		}

		int mid = p->cntChave / 2.0 ;
		*c = pDividir[mid] ;

		fprintf(flog, "Chave <%d> promovida\n", p->chaves[mid].id);
		for (j = 0; j < mid; j++) {
			p->chaves[j] = pDividir[j] ;
		}
		for (j = mid + 1; j < ORDEM; j++) {
			right->chaves[j - mid - 1] = pDividir[j] ;
		}

		free(pDividir) ;
		if (p->folha == '0') {
			int *filhos = (int *) malloc(sizeof(int) * (ORDEM + 1)) ;

			for (j = 0; j <= i; j++) {
				filhos[j] = p->filhos[j] ;
			}
			filhos[j] = *newChild ;
			for (j = i + 2; j < ORDEM + 1; j++) {
				filhos[j] = p->filhos[j - 1] ;
			}

			for (j = 0; j <= mid; j++) {
				p->filhos[j] = filhos[j] ;
			}
			for (j = mid + 1; j < ORDEM + 1; j++) {
				right->filhos[j - mid - 1] = filhos[j] ;
			}

			free(filhos) ;
		}

		p->cntChave = p->cntChave / 2.0 ;
		right->cntChave = ORDEM / 2.0 ;
		*newChild = right->rrn ;

		wrPage(bt, p, filename) ;
		wrPage(bt, right, filename) ;

		freePage(right) ;
		free(right) ;

		return 2 ;
	}

}

int insert(bTree *bt, int id, long offset, char* filename, FILE * flog) {
	chave c ;
	c.id = id ;
	c.offset = offset ;

	if (search(bt, id, filename) != -1) {
		return 0 ;
	}
	pagina *root = (pagina *) malloc(sizeof(pagina)) ;

	if (bt->raiz == -1) {
		startPage(bt, root) ;
		root->cntChave = 1 ;
		root->folha = '1' ;
		root->chaves[0] = c ;
		bt->raiz = root->rrn = 0 ;
		bt->nextRrn++ ;

		atuCab(bt, filename) ;
		wrPage(bt, root, filename) ;
	} else {
		loadPage(bt, root, bt->raiz, filename) ;

		int newChild = 0;
		if (insertRec(bt, root, &c, &newChild, filename, flog) == 2) {
			pagina *p = malloc(sizeof(pagina)) ;
			startPage(bt, p) ;
			p->folha = '0' ;
			p->cntChave = 1 ;
			p->chaves[0] = c ;
			p->rrn = bt->nextRrn++ ;
			p->filhos[0] = bt->raiz ;
			p->filhos[1] = newChild ;
			bt->raiz = p->rrn ;
			atuCab(bt, filename) ;
			wrPage(bt, p, filename) ;
			freePage(p) ;
			free(p) ;
		}
	}

	freePage(root) ;
	free(root) ;
	return 1 ;
}
