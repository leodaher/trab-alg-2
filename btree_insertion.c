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

    char *buffer = (char *) malloc(sizeof(char) * PAGE_TAM) ;
    buffer[0] = '\0' ;

    sprintf(buffer, "%s%c%0*d", buffer, p->folha, INT_TAM, p->cntChave) ;

    int i ;
    for (i = 0; i < p->cntChave; i++) {
        sprintf(buffer, "%s%0*d%0*ld", buffer, INT_TAM, p->chaves[i].id, LONG_TAM, p->chaves[i].offset) ;
    }

    if (p->folha == '0') {
        for (i = 0; i <= p->cntChave; i++) {
        	sprintf(buffer, "%s%0*d", buffer, INT_TAM, p->filhos[i]) ;
        }
    }

    fseek(bt->bTFile, rrnToOffset(p->rrn), SEEK_SET) ;
    fprintf(bt->bTFile, "%s%0*d", buffer, (int) (PAGE_TAM - strlen(buffer)), 0) ;
    fclose(bt->bTFile) ;
    free(buffer) ;

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


//*****************************************************************
//						Inserção
//*****************************************************************

int insertRec(bTree *bt, pagina *p, chave *c, int *newChild, char* filename, FILE * flog) {
	int i, j ;

	i = 0 ;

	//Posiciona o i na posição da maior chave menor que o id dado
	while (i < p->cntChave && p->chaves[i].id < c->id) {
		i++ ;
	}

	if (p->folha == '0') {
		//Se a página não for uma folha

		//Cria uma página para carregar o filho em que
		//provavelmente a chave está
		pagina *child = malloc(sizeof(pagina));
		if (p->filhos[i] > bt->nextRrn) {
			printf("Errado\n");
		}
		loadPage(bt, child, p->filhos[i], filename) ;

		//Tenta inserir a chave na página filho
		if (insertRec(bt, child, c, newChild, filename, flog) == 1) {
			//Se conseguiu sem propagação, libera a
			//memória da página filho e retorna 1
			freePage(child) ;
			free(child) ;
			return 1 ;
		}

		//Caso houve propagação, libera a memória da página filho
		//e tenta inserir a chave propaga na página atual
		//A chave propaga está em c e o filho da direita está em
		//newChild
		freePage(child) ;
		free(child) ;
	}

	if (p->cntChave < ORDEM - 1) {
		//Se a quantidade de chaves na página for menor
		//do que a ordem da àrvore menos 1, insere normalmente
		//a chave na página

		//Desloca todas as chaves maiores do que c.id para a direita
		for (j = p->cntChave; j > i; j--) {
			p->chaves[j] = p->chaves[j - 1] ;
		}
		//Insere c na página
		p->chaves[i] = *c ;

		if (p->folha == '0') {
			//Se a página não for uma folha, quer dizer que
			//alguma inserção propagou uma chave e um filho

			//Descola todos os filhos em posições maiores que i+1
			//para a direita
			for (j = p->cntChave + 1; j > i + 1; j--) {
				p->filhos[j] = p->filhos[j - 1] ;
			}
			//Atricui o filho propagado na direita de i
			p->filhos[i + 1] = *newChild ;
		}

		//Incrementa o número de chaves na página
		p->cntChave++ ;
		//Escreve a página em disco
		wrPage(bt, p, filename) ;

		return 1 ;
	} else {
		//Se a página está cheia, ocorre a divisão da mesma
		fprintf(flog, "Divisao de no - pagina %d\n", p->rrn);
		//Cria uma página para dividir as chaves
		pagina *right = (pagina *) malloc(sizeof(pagina)) ;
		startPage(bt, right) ;
		right->folha = p->folha ;
		right->rrn = bt->nextRrn++ ;
		atuCab(bt, filename) ;

		//Cria um vetor para ordenar as chaves antes de dividir
		chave *toSplit = (chave *) malloc(sizeof(chave) * ORDEM) ;

		//Coloca todas as chaves no vetor
		for (j = 0; j < i; j++) {
			toSplit[j] = p->chaves[j] ;
		}
		toSplit[i] = *c ;
		for (j = i; j < p->cntChave; j++) {
			toSplit[j + 1] = p->chaves[j] ;
		}

		//mid é a chave que vai ser promovida
		int mid = p->cntChave / 2.0 ;
		*c = toSplit[mid] ;

		fprintf(flog, "Chave <%d> promovida\n", p->chaves[mid].id);

		//Distribui as chaves entre as duas páginas
		for (j = 0; j < mid; j++) {
			p->chaves[j] = toSplit[j] ;
		}
		for (j = mid + 1; j < ORDEM; j++) {
			right->chaves[j - mid - 1] = toSplit[j] ;
		}

		free(toSplit) ;

		//Se a página não for folha, divide os filhos
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

		//Atualiza a quantidade de chaves em cada página
		p->cntChave = p->cntChave / 2.0 ;
		right->cntChave = ORDEM / 2.0 ;

		//Atualiza o filho propagado
		*newChild = right->rrn ;

		//Escreve no arquivo
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
		//Se a árvore não existe, cria uma página,
		//a raiz, com a chave a ser inserida
		startPage(bt, root) ;
		root->cntChave = 1 ;
		root->folha = '1' ;
		root->chaves[0] = c ;
		bt->raiz = root->rrn = 0 ;
		bt->nextRrn++ ;

		atuCab(bt, filename) ;
		wrPage(bt, root, filename) ;
	} else {
		//Se a árvore exite, carrega a raiz em memória
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
