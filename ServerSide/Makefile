CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm

all: ser

ser: server.c server.h
	 ${CC} -o server server.c

clean:
	rm -f *.o *~