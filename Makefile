CC=gcc
CFLAGS=-W -Wall
LDFLAGS=
EXEC=flc
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)
PREFIX:=/usr

all: $(EXEC)

fc: $(OBJ)
	@$(CC) -o $@ $^ $(LDFLAGS)
	
%.o: %.c
	@$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -f $(EXEC) *.o

install:
	install -m 755 $(EXEC) $(PREFIX)/bin/
