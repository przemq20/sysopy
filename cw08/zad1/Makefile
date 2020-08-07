CC=gcc
CFLAGS=-Wall -o

compile: main.c
	$(CC) $(CFLAGS) main main.c common.h -lpthread

test:
	make compile
	./main 1 block baboon.ascii.pgm result.txt
	./main 1 interleaved baboon.ascii.pgm result.txt
	./main 1 sign baboon.ascii.pgm result.txt
	./main 2 block baboon.ascii.pgm result.txt
	./main 2 interleaved baboon.ascii.pgm result.txt
	./main 2 sign baboon.ascii.pgm result.txt
	./main 4 block baboon.ascii.pgm result.txt
	./main 4 interleaved baboon.ascii.pgm result.txt
	./main 4 sign baboon.ascii.pgm result.txt
	./main 8 block baboon.ascii.pgm result.txt
	./main 8 interleaved baboon.ascii.pgm result.txt
	./main 8 sign baboon.ascii.pgm result.txt

clean:
	rm -f main