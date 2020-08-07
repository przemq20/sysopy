all:
	clear
	make clean
	make compile
	make setup_test_folders
	make test
	make clean
	
compile:
	gcc -Wall -g -c find.c
	gcc -o find find.o

setup_test_folders:
	rm -rf tests
	mkdir tests
	touch tests/one.txt
	touch tests/two.txt
	mkdir tests/directory
	touch tests/directory/three.txt
	mkdir tests/directory/four
	touch tests/directory/four/five.txt
	ln -s tests/two.txt tests/directory/six
	ln -s tests/directory/four/five.txt tests/seven

test:
	./find . sys

clean:
	rm -f *.o
	rm -f find
	rm -rf test