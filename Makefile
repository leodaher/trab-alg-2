all: t2.c
	gcc t2.c btree_search.c btree_insertion.c btree_create.c -o t3

run: all
	./t3

valgrind: all
	valgrind ./t3

valgrindFull: all
	valgrind --leak-check=full --show-leak-kinds=all ./tbt
