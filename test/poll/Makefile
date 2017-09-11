
CC=gcc
FLAGS= -I../../include -g
EXECPATH= ../../
OBJPATH= ../../src/
LIBPATH= ../../src/
LIBNAME=libarith.a
LFLAGS= -lhash -L$(LIBPATH)

default: pollserver clean


pollserver: server.o $(LIBPATH)libhash.a
	$(CC) -o $(EXECPATH)pollserver server.o $(LFLAGS) -g

server.o: server.c
	$(CC) $(FLAGS) -c server.c
 



clean:
	rm -rf server.o 

