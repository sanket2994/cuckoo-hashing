
CC=gcc
FLAGS= -I ../../include -g

OBJPATH= ../../src/

default: multithreadServer clean

multithreadServer: server.o hash.o
	$(CC) -o multithreadServer server.o hash.o -lpthread -g

server.o: server.c
	$(CC) $(FLAGS) -c server.c
 
hash.o: $(OBJPATH)hash.c
	$(CC) $(FLAGS) -c $(OBJPATH)hash.c

clean:
	rm -rf server.o hash.o































