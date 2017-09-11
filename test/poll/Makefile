
CC=gcc
FLAGS= -I../../include -g
EXECPATH= ../../
OBJPATH= ../../src/


default: pollserver clean

pollserver: server.o hash.o
	$(CC) -o $(EXECPATH)pollserver server.o hash.o -g

server.o: server.c
	$(CC) $(FLAGS) -c server.c
 
hash.o: $(OBJPATH)hash.c
	$(CC) $(FLAGS) -c $(OBJPATH)hash.c

clean:
	rm -rf server.o hash.o































