CC = gcc
FLAGS = -Wall

static:
	$(CC) $(FLAGS) -c library.c -o library.o
	ar rcs library.a library.o
	make clean

shared:
	$(CC) -fPIC $(FLAGS) -c library.c -o library.o
	$(CC) -fPIC $(FLAGS) -shared -o library.so library.o
	make clean

clean:
	rm -f *.o

clean_all:
	rm main tmp.txt library.a library.so