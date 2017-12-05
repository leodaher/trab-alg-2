#ifndef BTREE_INSERTION_H
#define BTREE_INSERTION_H

#include "btree.h"

int wrPage(bTree *bt, pagina *p, char* filename);
int atuCab(bTree *bt, char* filename);
int insertRec(bTree *bt, pagina *p, chave *c, int *newChild, char* filename, FILE * flog);
int insert(bTree *bt, int id, long offset, char* filename, FILE * flog) ;

#endif
