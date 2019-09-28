#makefile 
CC = gcc
main: main.c functions.c 
	$(CC) $(CFLAGS) $? $(LDFLAGS) -o $@
clear: rm -f main
