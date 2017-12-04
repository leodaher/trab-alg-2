/*  Trabalho 3 - Algoritimos e Estruturas de Dados II
 *
 *  Desenvolvedores:
 *    Guilherme Prearo
 *    Gustavo Nicolau Goncalves
 *    Pedro V. B. Jeronymo
 *
 *  (Compilador gcc 4.9.2)
 */

#include "trabalho3.h"
#include "btree.h"


int reg_to_buffer(registro_t *reg, char buffer[REGFILE_BUFFERSIZE])
{
	sprintf(buffer, "%d|%s|%s|", reg->id, reg->titulo, reg->genero);
	return strlen(buffer);
}

int file_to_reg(registro_t *reg, long offset, FILE *fd) {
	fseek(fd, offset, SEEK_SET) ;

	int size = 0;
	fscanf(fd, "%d", &size) ;

	if (size == 0) {
		return 0 ;
	}

	char *buffer = (char *) malloc(sizeof(char) * (size + 1)) ;
	fgets(buffer, size + 1, fd) ;

	char *p = strtok(buffer, "|") ;

	reg->id = atoi(p) ;
	p = strtok(NULL, "|") ;
	strcpy(reg->titulo, p) ;
	p = strtok(NULL, "|") ;
	strcpy(reg->genero, p) ;

	free(buffer) ;

	return 1 ;
}

long insert_reg(registro_t* reg, FILE* fdata)
{
	char buffer[REGFILE_BUFFERSIZE];
	char size = reg_to_buffer(reg, buffer);
	long offset = regfile_insert(buffer, size, fdata);
	return offset;
}

static void print_reg(registro_t *reg) {
	printf("ID: %d\n"
		   "Título: %s\n"
		   "Gênero: %s\n",
		   reg->id, reg->titulo,
		   reg->genero);
}

static void search_musica(FILE *fdata, bTree *bt, char* filename)
{
	int id ;
	printf("Informe o id da música a ser pesquisada: ");
	scanf("%d%*c", &id);

	long offset = search(bt, id, filename) ;
	if (offset == -1) {
		printf("Música não existente!\n");
		return ;
	}

	registro_t reg ;
	if (!file_to_reg(&reg, offset, fdata)) {
		printf("Música não existente!\n");
		return ;
	}

	print_reg(&reg) ;
}


static void insert_musica(FILE* fdata, bTree *bt, char* filename)
{
	registro_t reg;
	printf("\n\nInsira os dados da nova música\n");
	printf("Id: ");
	scanf("%d%*c", &reg.id);
	if (search(bt, reg.id, filename) != -1) {
		printf("Erro! Música existente!\n");
		return ;
	}
	printf("Título: ");
	fgets(reg.titulo, sizeof(reg.titulo), stdin);
	reg.titulo[strlen(reg.titulo)-1] = '\0';
	printf("Gênero: ");
	fgets(reg.genero, sizeof(reg.genero), stdin);
	reg.genero[strlen(reg.genero)-1] = '\0';

	long offset = insert_reg(&reg, fdata);
	insert(bt, reg.id, offset, filename) ;
	printf("\n<Música inserida com sucesso>\n");
}

static void remove_musica(FILE* fdata, bTree *bt, char* filename)
{
	printf("Indique o ID da música: ");
	int id;
	scanf("%d%*c", &id);
	long offset = search(bt, id, filename);
	if (offset == -1) {
		printf("Música não encontrada!\n") ;
		return ;
	}
	regfile_remove(offset, fdata);
	printf("\n<Música removida com sucesso>\n");
}


void create_index_read (FILE* fidx, bTree *bt, char *filename) {
	registro_t reg;
	char tamStr[4];
	int i = 0;
	
	char c = fgetc(fidx);

	if (c == EOF){
		printf("Termino da leitura\n");
		return;
	}
	long offset = ftell(fidx)-1;
	while (c != '|'){
		tamStr[i] = c;
		i++;
		c = fgetc(fidx);
	}
	int tam = atoi(tamStr);

	char *str = malloc(tam*sizeof(char));
	fgets(str, tam, fidx);

	char s[] = "|";
	reg.id = atoi(strtok(str, s));
	strcpy(reg.titulo, strtok(NULL, s));
	strcpy(reg.genero, strtok(NULL, s));

	printf("\n    tam = %d\n    id = %d\n    titulo = %s\n    genero = %s\n\n",tam, reg.id, reg.titulo, reg.genero);

	c = fgetc(fidx);
	
	insert(bt, reg.id, offset, "rola.idx");


	create_index_read(fidx, bt, filename);
	
}

void create_index (FILE *flog) {
	char filename[20];
	printf("Insira o nome do arquivo de dados: ");
	scanf("%s", filename);
	bTree btidx;

	FILE *fidx = fopen(filename, "r");
	
	initBT(&btidx, "rola.idx");
	fprintf(flog, "Execucao da criacao do arquivo de indice <rola.idx> com base no arquivo de dados <%s>.\n", filename);
	create_index_read(fidx, &btidx, filename);
	fclose(fidx);
}






static int ui(FILE* fdata, bTree *bt, char* filename, FILE *flog)
{
	printf("\n\n::Bem-vindo ao T3 de Alg2::\n\n");
	printf("Desenvolvedores:\n\tGuilherme Prearo\n\tGustavo N. Goncalves\n\tPedro V. B. Jeronymo\n\n\n");

	char run = 1;

	while(run)
	{
		int op;
		printf("\n\nSelecione uma operacao de acordo com seu codigo numerico:\n");
		printf("1. Inserir música\n2. Remover música\n3. Pesquisar por ID\n5. Fechar o programa\n");
		printf("\nOperacao:> ");
		scanf("%d%*c", &op);
		switch(op) {
		case 1:
			insert_musica(fdata, bt, filename);
			break;
		case 2:
			remove_musica(fdata, bt, filename);
			break;
		case 3:
			search_musica(fdata, bt, filename);
			break;
		case 4:
			create_index(flog);
			break;
		case 5:
			run = 0;
			break;
		default:
			printf("\n<Erro: selecione uma operacao valida>\n");
			break;
		}
	}

	return 0;
}

static int openfile(const char* filename, FILE** fd, FILE** flog)
{
	//*fd = fopen(filename, "rb+");
	*flog = fopen("log_ldaher.txt", "w");
	*fd = fopen(filename, "r+");
	if(!*fd)
	{
		//*fd = fopen(filename, "wb+");
		*fd = fopen(filename, "w+");
		if(!*fd)
		{
			return -1;
		}
	}
	return 0;
}

int main(void)
{
	FILE *fdata, *flog ;
	int err;

	bTree bt ;
	char *filename = BTFILE;
	initBT(&bt, filename);

	err = openfile(DATA_FILE, &fdata, &flog);
	if(err) return err;

	err = ui(fdata, &bt, filename, flog);
	if(err) return err;

	fclose(flog);
	fclose(fdata);
	return 0;
}
