
CC=gcc
FLAGS= -I../../include -g
EXECPATH= ../../

default: tcpclient udpclient clean

tcpclient: tcpclient.o 
	$(CC) -o $(EXECPATH)tcpclient tcpclient.o -g

udpclient: udpclient.o
	$(CC) -o $(EXECPATH)udpclient udpclient.o -g

tcpclient.o: tcpclient.c
	$(CC) -c $(FLAGS) tcpclient.c 

udpclient.o: udpclient.c
	$(CC) -c $(FLAGS) udpclient.c

clean:
	rm -rf *.o


















