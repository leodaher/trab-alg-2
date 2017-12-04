#include <stdio.h>
#include <stdlib.h>
#include "btree_create.h"
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

/*
	Nome: loadPage

	Parâmetros:
		bt - ponteiro para bTree
        p - ponteiro para página
        rrn - RRN da página no arquivo

	Função: Lê do arquivo a página com o rrn dado

	Retorno:
		1 - Sucesso
		0 - Arquivo de indice não existe
*/
int loadPage(bTree *bt, pagina *p, int rrn, char* filename) {
	bt->bTFile = fopen(filename, "r") ;
	if (!bt->bTFile || bt->raiz == -1) {
		//Arquivo não existe
		p->cntChave = 0 ;

		return 0 ;
	}

	startPage(bt, p) ;

    //Cria um buffer para ler todos os dados da página com apenas um read no arquivo
	char *buffer = (char *) malloc(sizeof(char) * (PAGE_TAM + 1)) ;

    //Posiciona o ponteiro do arquivo na posição da página
	fseek(bt->bTFile, rrnToOffset(rrn), SEEK_SET) ;
    //Lê a página inteira em memória
	fgets(buffer, PAGE_TAM + 1, bt->bTFile) ;
    //Fecha o arquivo
	fclose(bt->bTFile) ;

	//Posição de leitura do buffer
	char *pos = buffer ;

    //Lê um char par identificar se a página é folha ou não
    //e lê um int para saber quantas chaves há na página
	sscanf(pos, "%c%8d", &p->folha, &p->cntChave) ;
    //Avança no buffer
	pos += INT_TAM + CHAR_TAM ;

	int i ;
	for (i = 0; i < p->cntChave; i++) {
        //Lê todas as chaves e offsets
		sscanf(pos, "%8d%15ld", &p->chaves[i].id, &p->chaves[i].offset) ;
		pos += INT_TAM + LONG_TAM;
	}

	if (p->folha == '0') {
        //Se a página não for folha, lê os filhos
		for (i = 0; i <= p->cntChave; i++) {
			sscanf(pos, "%8d", &p->filhos[i]) ;
			pos += INT_TAM ;
		}
	}

    //Libera a memória do buffer
	free(buffer) ;

    p->rrn = rrn ;
	return 1 ;
}

/*
	Nome: freePage

	Parâmetros:
        p - ponteiro para página

	Função: Libera a mnemória alocada de uma página
*/
void freePage(pagina *p) {

    //Libera memória das chaves
	free(p->chaves) ;

	if (p->folha == '0') {
        //Se a página não for folha, libera a memória dos filhos
		free(p->filhos) ;
	}
}
