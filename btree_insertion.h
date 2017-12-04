#include "btree.h"


int wrPage(bTree *bt, pagina *p, char* filename);
int atuCab(bTree *bt, char* filename);
int insertRec(bTree *bt, pagina *p, chave *c, int *newChild, char* filename, FILE * flog);
// Insere uma chave no arquivo de índices
int insert(bTree *bt, int id, long offset, char* filename, FILE * flog) ;
