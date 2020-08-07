CC = gcc
FLAGS = -Wall

compile: main.c
	$(CC) $(FLAGS) -o main main.c -lpthread -lm

test:
	make main
	./main 2 10

clean:
	rm -f main