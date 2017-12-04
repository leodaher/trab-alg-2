#include "trabalho3.h"
#include "btree.h"


int readRegister(registro_t *reg, long offset, FILE *fd) {
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

static void printRegister(registro_t *reg) {
	printf("ID: %d\n"
		   "Título: %s\n"
		   "Gênero: %s\n",
		   reg->id, reg->titulo,
		   reg->genero);
}

static void searchMusica(FILE *fdata, bTree *bt, char* filename, FILE *flog)
{
	int id ;
	printf("Informe o id da música a ser pesquisada: ");
	scanf("%d%*c", &id);

	fprintf(flog, "Execucao de operacao de PESQUISA de <%d>\n", id);

	long offset = search(bt, id, filename) ;
	if (offset == -1) {
		printf("Música não existente!\n");
		fprintf(flog, "“Chave <%d> nao encontrada\n", id);
		return ;
	}

	registro_t reg ;
	if (!readRegister(&reg, offset, fdata)) {
		printf("Música não existente!\n");
		fprintf(flog, "Chave <%d> nao encontrada\n", id);
		return ;
	}
	fprintf(flog, "Chave <%d> encontrada, offset <%ld>, Titulo: <%s>, Genero: <%s>\n", reg.id, offset, reg.titulo, reg.genero);
	printRegister(&reg) ;
}

/////////////////////////////////////////////////////////////////////////////////////////

long writeInReg(char buffer[REGFILE_BUFFERSIZE], char size, FILE* fd)
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



static void insertMusica(FILE* fdata, bTree *bt, char* filename, FILE * flog)
{
	registro_t reg;
	printf("\n\nInsira os dados da nova música\n");
	printf("Id: ");
	scanf("%d%*c", &reg.id);

	printf("Título: ");
	fgets(reg.titulo, sizeof(reg.titulo), stdin);
	reg.titulo[strlen(reg.titulo)-1] = '\0';
	printf("Gênero: ");
	fgets(reg.genero, sizeof(reg.genero), stdin);
	reg.genero[strlen(reg.genero)-1] = '\0';

	fprintf(flog, "Execucao de operacao de INSERCAO de <%d>, <%s>, <%s>\n", reg.id, reg.titulo, reg.genero);

	if (search(bt, reg.id, filename) != -1) {
		printf("Erro! Música existente!\n");
		fprintf(flog, "Chave <%d> duplicada\n", reg.id);
		return ;
	}

	char buffer[REGFILE_BUFFERSIZE];
	sprintf(buffer, "%d|%s|%s|", reg.id, reg.titulo, reg.genero);
	char size = strlen(buffer);
	long offset = writeInReg(buffer, size, fdata);

	insert(bt, reg.id, offset, filename, flog) ;
	fprintf(flog, "Chave <%d> inserida com sucesso\n", reg.id);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void createIndexRead (FILE* fidx, bTree *bt, char *filename,  char *idxname, FILE * flog) {
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

	insert(bt, reg.id, offset, idxname, flog);


	createIndexRead(fidx, bt, filename, idxname, flog);

}

void createIndex (FILE *flog) {
	char filename[20], idxname[20];
	printf("Insira o nome do arquivo de dados: ");
	scanf("%s", filename);
	bTree btidx;
	FILE *fidx = fopen(filename, "r");

	int i = 0;
	while (filename[i] != '.') {
		idxname[i] = filename[i];
		i++;
	}
	char *idx = ".idx";
	for(int j = 0; j < 5; j++)
		idxname[i+j] = idx[j];


	startBTree(&btidx, idxname);
	fprintf(flog, "Execucao da criacao do arquivo de indice <%s> com base no arquivo de dados <%s>.\n", idxname, filename);
	createIndexRead(fidx, &btidx, filename, idxname, flog);
	fclose(fidx);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int menu(FILE* fdata, bTree *bt, char* filename, FILE *flog)
{
	printf("\n-- TRABALHO 2 DE ALGORITMOS E ESTRUTURAS DE DADOS II --\n\n");
	printf("Integrantes:\n- Carlos Henrique de Oliveira Franco\n- Guilherme Brunassi Nogima\n- Joao Pedro Silva Mambrini Ruiz\n- Leonardo Akel Daher\n\n");

	char run = 1;

	while(run)
	{
		int op;
		printf("\n\n1. Criar índice\n2. Inserir música\n3. Pesquisar por ID\n4. Fechar o programa\n");
		printf("\nComando: ");
		scanf("%d%*c", &op);
		switch(op) {
		case 1:
			createIndex(flog);
			break;
		case 2:
			insertMusica(fdata, bt, filename, flog);
			break;
		case 3:
			searchMusica(fdata, bt, filename, flog);
			break;
		case 4:
			run = 0;
			break;
		default:
			printf("\nERRO\n");
			break;
		}
	}

	return 0;
}

static int openFile(const char* filename, FILE** fd)
{
	//*fd = fopen(filename, "rb+");
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
	startBTree(&bt, filename);

	flog = fopen("log_ldaher.txt", "a");

	err = openFile(DATA_FILE, &fdata);
	if(err) return err;

	err = menu(fdata, &bt, filename, flog);
	if(err) return err;

	fclose(flog);
	fclose(fdata);
	return 0;
}
