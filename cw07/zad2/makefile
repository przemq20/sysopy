CC = gcc -Wall -std=gnu11 -ggdb
LIB = semaphore.o sharedArray.o sharedCounter.o -lm -lrt -lpthread

libraries:
	$(CC) -o semaphore.o -c semaphore.c
	$(CC) -o sharedArray.o -c sharedArray.c
	$(CC) -o sharedCounter.o -c sharedCounter.c

compile:
	make libraries
	$(CC) -o main main.c $(LIB)
	$(CC) -o worker1 worker1.c $(LIB)
	$(CC) -o worker2 worker2.c $(LIB)
	$(CC) -o worker3 worker3.c $(LIB)

test:
	make compile
	./main 5 4 2

clean:
	rm main
	rm worker1
	rm worker2
	rm worker3
	rm *.o
