
CC=gcc
FLAGS= -I../../include

OBJPATH= ../../src/

default: rawServer clean

rawServer: server.o hash.o
	$(CC) -o rawServer server.o hash.o -g

server.o: server.c
	$(CC) $(FLAGS) -c server.c 
 
hash.o: $(OBJPATH)hash.c
	$(CC) $(FLAGS) -c $(OBJPATH)hash.c 

clean:
	rm -rf server.o hash.o































