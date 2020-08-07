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
	rm -rf test
	mkdir test
	touch test/one.txt
	touch test/two.txt
	mkdir test/directory
	touch test/directory/three.txt
	mkdir test/directory/four
	touch test/directory/dirtwo/five.txt
	ln -s dirtwo/second_level.txt test/directory/six
	ln -s directory/dirtwo/five.txt test/seven

test:
	./find 'test' -mtime -2 -atime -10 -maxdepth 3  sys
	./find './test' -mtime -2 -atime -10 -maxdepth 3 nftw

clean:
	rm -f *.o
	rm -f find
	rm -rf test