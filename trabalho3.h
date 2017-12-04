#ifndef TRABALHO3_H_INCLUDED
#define TRABALHO3_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "regfile.h"

#define BTREE_FILE	"arvore.idx"
#define DATA_FILE	"dados.dad"

/* Tamanho do maior inteiro que cabe em um char + '\0' */
#define REGFILE_BUFFERSIZE 256

/* Marcador de registro removido */
#define REGFILE_REGREMOVED "*|"

typedef struct {
	int id;
	char titulo[30];
	char genero[20];
} registro_t;

#endif
