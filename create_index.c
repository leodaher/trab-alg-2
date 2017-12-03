void create_index_read (FILE* fidx, bTree *bt) {
	registro_t reg;
	char tamStr[4];
	int i = 0;
	
	char c = fgetc(fidx);
	if (c == EOF){
		printf("Termino da leitura\n");
		return;
	}
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


	printf("\n    tam = %d\n", tam);
	printf("    id = %d\n    titulo = %s\n    genero = %s\n\n", reg.id, reg.titulo, reg.genero);

	c = fgetc(fidx);

	//long offset = insert_reg(&reg, fdata);
	//insert(bt, reg.id, offset) ;

	create_index_read(fidx, bt);

	
}

void create_index (FILE* fdata, bTree *bt) {
	char *filename;
	printf("Insira o nome do arquivo: ");
	scanf("%s", filename);
	FILE *fidx = fopen(filename, "r");;
	create_index_read(fidx, bt);
	fclose(fidx);
}