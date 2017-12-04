/*  Trabalho 3 - Algoritimos e Estruturas de Dados II
 *
 *  Desenvolvedores:
 *    Guilherme Prearo
 *    Gustavo Nicolau Goncalves
 *    Pedro V. B. Jeronymo
 *
 *  (Compilador gcc 4.9.2)
 */

#include "regfile.h"


int regfile_parse_buffer(char buffer[REGFILE_BUFFERSIZE], int ini)
{
	int i = 0;
	/* Encontra delimitador */
	while(buffer[ini+i] != '|') i++;
	/* Insere finalizador de string */
	buffer[ini+i] = '\0';
	/* Retorna indice de inicio do proximo campo */
	return i+1;
}

void regfile_remove(long offset, FILE* fd)
{
	fseek(fd, offset, SEEK_SET);
	//fwrite(REGFILE_REGREMOVED, sizeof(char), sizeof(REGFILE_REGREMOVED), fd);
	fprintf(fd, "%s", REGFILE_REGREMOVED);
}

long regfile_insert_at_end(char buffer[REGFILE_BUFFERSIZE], char size, FILE* fd)
{
	fseek(fd, 0, SEEK_END);
	long offset = ftell(fd);
	/* Escreve tamanho do registro */
	//fwrite(&size, sizeof(char), 1, fd);
	fprintf(fd, "%d", size) ;
	/* Escreve registro */
	//fwrite(buffer, sizeof(char), size, fd);
	fprintf(fd, "|%s", buffer) ;

	return offset;
}

long regfile_insert(char buffer[REGFILE_BUFFERSIZE], char size, FILE* fd)
{
	return regfile_insert_at_end(buffer, size, fd);
}

