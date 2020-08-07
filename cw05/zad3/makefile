CC = gcc -Wall -std=gnu11 -ggdb

all:
	make compile
	make test
	make clean

compile:
	$(CC) -o producer producer.c
	$(CC) -o consumer consumer.c
	$(CC) -o tester tester.c

test:
	./tester

clean:
	rm consumer
	rm producer
	rm tester
	rm fifo

