

CC=gcc

CFLAGS=-I../include -c

LIBNAME= libhash.a

archive: hash.o
	ar -crv $(LIBNAME) hash.o
	rm -rf hash.o
hash.o: hash.c
	$(CC) $(CFLAGS) hash.c
