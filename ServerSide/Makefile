CFLAGS = -c -Wall
CC = gcc
LIBS =  -lssl -lcrypto

all: ser

ser: server.c
	 ${CC} -o server server.c ${LIBS}

clean:
	rm -f *.o *~