/*  Trabalho 3 - Algoritimos e Estruturas de Dados II
 *
 *  Desenvolvedores:
 *    Guilherme Prearo
 *    Gustavo Nicolau Goncalves
 *    Pedro V. B. Jeronymo
 *
 *  (Compilador gcc 4.9.2)
 */
 
#ifndef REGFILE_H_INCLUDED
#define REGFILE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Tamanho do maior inteiro que cabe em um char + '\0' */
#define REGFILE_BUFFERSIZE 256

/* Marcador de registro removido */
#define REGFILE_REGREMOVED "*|"

/* Tipos de usuario */
#define TU_GRATUITO	0
#define TU_COMUM	1
#define TU_PREMIUM	2

int regfile_parse_buffer(char buffer[REGFILE_BUFFERSIZE], int ini);
void regfile_remove(long offset, FILE* fd);
long regfile_insert_at_end(char buffer[REGFILE_BUFFERSIZE], char size, FILE* fd);
long regfile_insert(char buffer[REGFILE_BUFFERSIZE], char size, FILE* fd);

#endif