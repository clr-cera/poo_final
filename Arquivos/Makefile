all: main.o funcoes_fornecidas.o functionalities.o header.o register.o tools.o index.o
	gcc main.o funcoes_fornecidas.o functionalities.o header.o register.o tools.o index.o -o main -std=c99 -Wall -g
	rm *.o

main.o:
	gcc -c main.c -o main.o -g

functionalities.o:
	gcc -c functionalities.c -o functionalities.o -g

index.o:
	gcc -c index_files/index.c -o index.o -g

header.o:
	gcc -c binary_files/header.c -o header.o -g

register.o:
	gcc -c binary_files/register.c -o register.o -g

funcoes_fornecidas.o:
	gcc -c utilities/funcoes_fornecidas.c -o funcoes_fornecidas.o -g

tools.o:
	gcc -c utilities/tools.c -o tools.o -g

clean:
	rm *.o main

test:
	make clean
	make all
	./main <2.in>1.out

valgrind: clean all
	valgrind --leak-check=full --show-leak-kinds=all ./main<2.in

zip:
	zip -r Arquivos.zip main.c functionalities.c index_files/index.c binary_files/header.c binary_files/register.c utilities/funcoes_fornecidas.c utilities/tools.c funcionalidades.h index_files/index.h binary_files/header.h binary_files/register.h utilities/funcoes_fornecidas.h utilities/tools.h Makefile

run:
	./main
