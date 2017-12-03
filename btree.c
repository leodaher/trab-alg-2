/*  Trabalho 3 - Algoritimos e Estruturas de Dados II
 *
 *  Desenvolvedores:
 *    Guilherme Prearo
 *    Gustavo Nicolau Goncalves
 *    Pedro V. B. Jeronymo
 *
 *  (Compilador gcc 4.9.2)
 */

#include "btree.h"

#include <string.h>

void printBucket(pagina *p) ;

int initBT(bTree *bt) {
	bt->ordem = ORDEM ;

	bt->bTFile = fopen(BTFILE, "r") ;
	if (!bt->bTFile) {
		//Arquivo da árvore não existe
		bt->raiz = -1 ;
		bt->nextRrn = 0 ;

		bt->bTFile = fopen(BTFILE, "w") ;
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

void initBucket(bTree *bt, pagina *p) {
	p->cntChave = 0 ;
	p->chaves = (chave *) malloc(sizeof(chave) * (bt->ordem - 1)) ;
	p->filhos = (int *) malloc(sizeof(int) * bt->ordem) ;
}


int rrnToOffset(int rrn) {
	return 2 * INT_SIZE + rrn * PAGE_SIZE ;
}

/*
	Nome: loadBucket

	Parâmetros:
		bt - ponteiro para bTree
        p - ponteiro para página
        rrn - RRN da página no arquivo

	Função: Lê do arquivo a página com o rrn dado

	Retorno:
		1 - Sucesso
		0 - Arquivo de indice não existe
*/
int loadBucket(bTree *bt, pagina *p, int rrn) {
	bt->bTFile = fopen(BTFILE, "r") ;
	if (!bt->bTFile || bt->raiz == -1) {
		//Arquivo não existe
		p->cntChave = 0 ;

		return 0 ;
	}

	initBucket(bt, p) ;

    //Cria um buffer para ler todos os dados da página com apenas um read no arquivo
	char *buffer = (char *) malloc(sizeof(char) * (PAGE_SIZE + 1)) ;

    //Posiciona o ponteiro do arquivo na posição da página
	fseek(bt->bTFile, rrnToOffset(rrn), SEEK_SET) ;
    //Lê a página inteira em memória
	fgets(buffer, PAGE_SIZE + 1, bt->bTFile) ;
    //Fecha o arquivo
	fclose(bt->bTFile) ;

	//Posição de leitura do buffer
	char *pos = buffer ;

    //Lê um char par identificar se a página é folha ou não
    //e lê um int para saber quantas chaves há na página
	sscanf(pos, "%c%8d", &p->folha, &p->cntChave) ;
    //Avança no buffer
	pos += INT_SIZE + CHAR_SIZE ;

	int i ;
	for (i = 0; i < p->cntChave; i++) {
        //Lê todas as chaves e offsets
		sscanf(pos, "%8d%15ld", &p->chaves[i].id, &p->chaves[i].offset) ;
		pos += INT_SIZE + LONG_SIZE;
	}	

	if (p->folha == '0') {
        //Se a página não for folha, lê os filhos
		for (i = 0; i <= p->cntChave; i++) {
			sscanf(pos, "%8d", &p->filhos[i]) ;
			pos += INT_SIZE ;
		}
	}
	
    //Libera a memória do buffer
	free(buffer) ;

    p->rrn = rrn ;
	return 1 ;
}

/*
	Nome: freeBucket

	Parâmetros:
        p - ponteiro para página

	Função: Libera a mnemória alocada de uma página
*/
void freeBucket(pagina *p) {

    //Libera memória das chaves
	free(p->chaves) ;

	if (p->folha == '0') {
        //Se a página não for folha, libera a memória dos filhos
		free(p->filhos) ;
	}
}


//*****************************************************************
//							Busca
//*****************************************************************

int searchRec(bTree *bt, pagina *p, chave *c) {
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
		loadBucket(bt, p, p->filhos[i]) ;
		return searchRec(bt, p, c) ;
	}
}

long search(bTree *bt, int id) {
	if (bt->raiz == -1) {
		return -1 ;
	}

	pagina p ;
	chave c ;
	c.id = id ;
	
	loadBucket(bt, &p, bt->raiz) ;

	if (!searchRec(bt, &p, &c)) {
		freeBucket(&p) ;
		return -1 ;
	}


	return c.offset ;

}

//*****************************************************************


int writeBucket(bTree *bt, pagina *p) {
    bt->bTFile = fopen(BTFILE, "r+") ;

    if (!bt->bTFile) {
        return 0 ;    
    }

    char *buffer = (char *) malloc(sizeof(char) * PAGE_SIZE) ;  
    buffer[0] = '\0' ;

    sprintf(buffer, "%s%c%0*d", buffer, p->folha, INT_SIZE, p->cntChave) ;

    int i ;
    for (i = 0; i < p->cntChave; i++) {
        sprintf(buffer, "%s%0*d%0*ld", buffer, INT_SIZE, p->chaves[i].id, LONG_SIZE, p->chaves[i].offset) ;   
    }

    if (p->folha == '0') {
        for (i = 0; i <= p->cntChave; i++) {
        	sprintf(buffer, "%s%0*d", buffer, INT_SIZE, p->filhos[i]) ;  
        }  
    }

    fseek(bt->bTFile, rrnToOffset(p->rrn), SEEK_SET) ;
    fprintf(bt->bTFile, "%s%0*d", buffer, (int) (PAGE_SIZE - strlen(buffer)), 0) ;
    fclose(bt->bTFile) ;
    free(buffer) ;

    return 1 ;
}

void printBucket(pagina *p) {
	if (p->cntChave == 0) {
		return ;
	}

	printf("Folha: %s\n"
		   "Quantidade de chaves: %d\n"
		   , p->folha == '1' ? "Sim" : "Não", p->cntChave);
	printf("IDs: \n") ;
	int i ;
	for (i = 0; i < p->cntChave; i++) {
		printf("%7d ", p->chaves[i].id) ;
	}

	printf("\nOffsets: \n") ;
	for (i = 0; i < p->cntChave; i++) {
		printf("%7ld ", p->chaves[i].offset) ;
	}
	printf("\n");

	if (p->folha == '1') {
		return ;
	}

	printf("Filhos: \n") ;
	for (i = 0; i <= p->cntChave; i++) {
		printf("%7d ", p->filhos[i]) ;
	}

	printf("\n");
}	

int updateHeader(bTree *bt) {
	bt->bTFile = fopen(BTFILE, "r+") ;

    if (!bt->bTFile) {
        return 0 ;    
    }

    fprintf(bt->bTFile, "%0*d%0*d", INT_SIZE, bt->raiz, INT_SIZE, bt->nextRrn) ;

    fclose(bt->bTFile) ;
    return 1 ;
}


//*****************************************************************
//						Inserção
//*****************************************************************

int insertRec(bTree *bt, pagina *p, chave *c, int *newChild) {
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
		loadBucket(bt, child, p->filhos[i]) ;

		//Tenta inserir a chave na página filho
		if (insertRec(bt, child, c, newChild) == 1) {
			//Se conseguiu sem propagação, libera a 
			//memória da página filho e retorna 1
			freeBucket(child) ;
			free(child) ;
			return 1 ;
		}

		//Caso houve propagação, libera a memória da página filho
		//e tenta inserir a chave propaga na página atual 
		//A chave propaga está em c e o filho da direita está em 
		//newChild
		freeBucket(child) ;
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
		writeBucket(bt, p) ;

		return 1 ;
	} else {
		//Se a página está cheia, ocorre a divisão da mesma

		//Cria uma página para dividir as chaves
		pagina *right = (pagina *) malloc(sizeof(pagina)) ;
		initBucket(bt, right) ;
		right->folha = p->folha ;
		right->rrn = bt->nextRrn++ ;
		updateHeader(bt) ;

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
		writeBucket(bt, p) ;
		writeBucket(bt, right) ;

		freeBucket(right) ;
		free(right) ;

		return 2 ;
	}

}

int insert(bTree *bt, int id, long offset) {
	chave c ;
	c.id = id ;
	c.offset = offset ;

	if (search(bt, id) != -1) {
		return 0 ;
	}

	pagina *root = (pagina *) malloc(sizeof(pagina)) ;

	if (bt->raiz == -1) {
		//Se a árvore não existe, cria uma página,
		//a raiz, com a chave a ser inserida
		initBucket(bt, root) ;
		root->cntChave = 1 ;
		root->folha = '1' ;
		root->chaves[0] = c ;
		bt->raiz = root->rrn = 0 ;
		bt->nextRrn++ ;

		updateHeader(bt) ;
		writeBucket(bt, root) ;
	} else {
		//Se a árvore exite, carrega a raiz em memória
		loadBucket(bt, root, bt->raiz) ;

		int newChild = 0;
		if (insertRec(bt, root, &c, &newChild) == 2) {
			pagina *p = malloc(sizeof(pagina)) ;
			initBucket(bt, p) ;

			p->folha = '0' ;
			p->cntChave = 1 ;
			p->chaves[0] = c ;
			p->rrn = bt->nextRrn++ ;
			p->filhos[0] = bt->raiz ;
			p->filhos[1] = newChild ;
			bt->raiz = p->rrn ;
			updateHeader(bt) ;

			writeBucket(bt, p) ;
			freeBucket(p) ;
			free(p) ;
		}
	}

	freeBucket(root) ;
	free(root) ;

	return 1 ;

}