CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm

all: cli

cli: client.c client.h
	 ${CC} -o client client.c

clean:
	rm -f *.o *~