CFLAGS = -c -Wall
CC = gcc
LIBS =  

all: cli

cli: client.c lib/client.h
	 ${CC} -o client client.c

clean:
	rm -f *.o *~