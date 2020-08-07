CC=gcc -Wall

all: build

build: main.c
	$(CC) -o main main.c
	$(CC) -o child child.c

clean: main
	rm main child
