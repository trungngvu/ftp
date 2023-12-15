CFLAGS = -c -Wall
CC = gcc
LIBS =  -lssl -lcrypto

all: ser

install: 
	sudo apt install libssl-dev zip unzip

ser: server.c lib/server.h
	 ${CC} -o server server.c ${LIBS}

clean:
	rm -f *.o *~