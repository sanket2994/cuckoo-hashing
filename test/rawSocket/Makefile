
CC=gcc
FLAGS= -I../../include
EXECPATH= ../../
OBJPATH= ../../src/

default: rawServer clean

rawServer: server.o hash.o
	$(CC) -o $(EXECPATH)rawServer server.o hash.o -g

server.o: server.c
	$(CC) $(FLAGS) -c server.c 
 
hash.o: $(OBJPATH)hash.c
	$(CC) $(FLAGS) -c $(OBJPATH)hash.c 

clean:
	rm -rf server.o hash.o































