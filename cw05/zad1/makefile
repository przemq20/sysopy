CC = gcc -Wall -std=gnu11 -ggdb

all: main

main: 
	$(CC) main.c -o main


test:
	rm -f commands.txt
	echo "ls -al | sort | head -3" >> commands.txt
	echo "ps -aux | sort | head -4" >> commands.txt
	echo "ls -l | wc -l" >> commands.txt
	./main commands.txt

clean:
	rm -Rf commands.txt main
