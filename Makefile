CC=gcc
CFLAGS=-W -Wall -ansi -pedantic
LDFLAGS=
EXEC=flc
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
PREFIX:=/usr/local

all: $(EXEC)

fc: $(OBJ)
	@$(CC) -o $@ $^ $(LDFLAGS)
	
%.o: %.c
	@$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -f $(EXEC) *.o

install:
	chmod 755 $(EXEC)
	cp $(EXEC) $(PREFIX)/bin/
