# NOMBRE DEL EJECUTABLE DEL TP
EXEC =  tp1
CC = gcc
CFLAGS = -Wall -Werror -pedantic -std=c99 -g
BIN = $(filter-out $(EXEC).c, $(wildcard *.c))
BINFILES = $(BIN:.c=.o)

all: main

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

ship_tar: clean_all
	tar -czf entrega.tar.gz Makefile *.c *.h *.pdf

ship_zip: clean_all
	zip entrega.zip Makefile *.c *.h *.pdf
	
main: $(BINFILES)  $(EXEC).c
	$(CC) $(CFLAGS) $(BINFILES) $(EXEC).c -o $(EXEC)

clean:
	rm -f $(wildcard *.o)

clean_all:
	rm -f $(wildcard *.o) $(EXEC)
	rm -f entrega.tar.gz
	rm -f entrega.zip

.PHONY: clean clean_all main ship_tar ship_zip
