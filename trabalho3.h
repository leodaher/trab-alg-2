/*  Trabalho 3 - Algoritimos e Estruturas de Dados II
 *
 *  Desenvolvedores:
 *    Guilherme Prearo
 *    Gustavo Nicolau Goncalves
 *    Pedro V. B. Jeronymo
 *
 *  (Compilador gcc 4.9.2)
 */

#ifndef TRABALHO3_H_INCLUDED
#define TRABALHO3_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "regfile.h"

#define BTREE_FILE	"arvore.idx"
#define DATA_FILE	"dados.dad"

typedef struct {
	int id;
	char titulo[30];
	char genero[20];
} registro_t;

#endif
