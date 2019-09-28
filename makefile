#makefile
CC = gcc
all:main.o functions.o
	$(CC) -o main main.o functions.o
main.o:main.c
	$(CC) -c main.c
functions.o:functions.c
	$(CC) -c functions.c
clear: 
	rm -f *.o all