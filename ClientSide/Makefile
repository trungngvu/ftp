CFLAGS = -c -Wall
CC = gcc
LIBS =  -lssl -lcrypto

all: cli

install: 
	sudo apt install zip unzip

cli: client.c lib/client.h
	 ${CC} -o client client.c ${LIBS}

clean:
	rm -f *.o *~