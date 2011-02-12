CC=gcc
CFLAGS=-W -Wall -ansi -pedantic
LDFLAGS=
EXEC=fc
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

all: $(EXEC)

fc: $(OBJ)
	@$(CC) -o $@ $^ $(LDFLAGS)
	
%.o: %.c
	@$(CC) -o $@ -c $< $(CFLAGS)

clean:
	rm -f $(EXEC) *.o

