
CC=gcc
FLAGS= -I../../include -g
EXECPATH= ../../
OBJPATH= ../../src/
LIBNAME= libhash.a
LIBPATH= ../../src/
LFLAGS= -lhash -L$(LIBPATH)

default: rawServer clean

rawServer: server.o $(LIBPATH)$(LIBNAME)
	$(CC) -o $(EXECPATH)rawServer server.o $(LFLAGS) -g

server.o: server.c
	$(CC) $(FLAGS) -c server.c 

clean:
	rm -rf server.o hash.o

