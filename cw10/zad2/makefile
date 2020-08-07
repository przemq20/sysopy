CC=gcc
CFLAGS=-c -Wall

compile:
	$(CC) $(CFLAGS) server.c
	$(CC) server.o -o server -lpthread 
	$(CC) $(CFLAGS) client.c
	$(CC) client.o -o client -lpthread 
	
test:
	make compile
	./server 3000 unix_socket
	$(info You have to launch clients manually by using these commands manually in separate terminals)
	$(info 	./client user1 UNIX unix_socket)
	$(info ./client user2 UNIX unix_socket)
	$(info ./client user3 INET 127.0.0.1:3000)
	$(info ./client user4 INET 127.0.0.1:3000)

clean:
	rm -f server
	rm -f client
	rm -f unix_socket
	rm -f *.o
