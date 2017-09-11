
CC=gcc
FLAGS= -I ../../include -g
EXECPATH= ../../
OBJPATH= ../../src/
LIBPATH= ../../src/
LIBNAME= libhash.a
LFLAGS= -lhash -L$(LIBPATH)


default: multithreadServer clean

multithreadServer: server.o $(LIBPATH)$(LIBNAME)
	$(CC) -o $(EXECPATH)multithreadServer server.o $(LFLAGS) -lpthread -g

server.o: server.c
	$(CC) $(FLAGS) -c server.c

clean:
	rm -rf server.o

