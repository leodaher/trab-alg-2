#ifndef BTREE_CREATE_H
#define BTREE_CREATE_H

#include "btree.h"

int startBTree(bTree *bt, char* filename);
void startPage(bTree *bt, pagina *p);
int rrnToOffset(int rrn);
int loadPage(bTree *bt, pagina *p, int rrn, char* filename);
void freePage(pagina *p);

#endif
