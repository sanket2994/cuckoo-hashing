
CC=gcc
FLAGS= -I../../include -g


default: tcpclient udpclient clean

tcpclient: tcpclient.o 
	$(CC) -o tcpclient tcpclient.o -g

udpclient: udpclient.o
	$(CC) -o udpclient udpclient.o -g

tcpclient.o: tcpclient.c
	$(CC) -c $(FLAGS) tcpclient.c 

udpclient.o: udpclient.c
	$(CC) -c $(FLAGS) udpclient.c

clean:
	rm -rf *.o


















