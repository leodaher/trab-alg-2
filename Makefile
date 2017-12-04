all: trabalho3.c
	gcc btree.c trabalho3.c -o t3

run: all
	./t3

valgrind: all
	valgrind ./t3

valgrindFull: all
	valgrind --leak-check=full --show-leak-kinds=all ./tbt
