zCC = gcc
FLAGS = -std=c99 -Wall
LIB_PATH = ../zad1
TEST_PATH = ../zad2
TEST_SOURCE_NAME = main.c
O = O1
export LD_LIBRARY_PATH=$(LIB_PATH)

static:
	$(CC) $(FLAGS) $(TEST_PATH)/$(TEST_SOURCE_NAME) -o main -$O -I$(LIB_PATH) $(LIB_PATH)/library.a 

shared:
	$(CC) $(FLAGS) $(TEST_PATH)/$(TEST_SOURCE_NAME) -o main -$O -I$(LIB_PATH) -L$(LIB_PATH) -lrary 

clean:
	rm -f *.o
	
dynamic:
	$(CC) $(FLAGS) ../zad3a/dynamic_main.c -o main -$O -I$(LIB_PATH)  -ldl -D DYNAMIC	

test:
	echo "\n\n-------Creating big array-------" >> raport3b.txt
	./main create_table 1000
	echo "\n\n-------Comparing Small files-------" >> raport3b.txt
	./main create_table 10 compare_pairs test_files/d.txt:test_files/b.txt test_files/d.txt:test_files/e.txt test_files/e.txt:test_files/b.txt
	echo "\n\n-------Comparing medium files-------" >> raport3b.txt
	./main create_table 10 compare_pairs test_files/c.txt:test_files/f.txt test_files/c.txt:test_files/g.txt test_files/f.txt:test_files/g.txt
	echo "\n\n-------Comparing big files-------" >> raport3b.txt
	./main create_table 10 compare_pairs test_files/a.txt:test_files/h.txt test_files/h.txt:test_files/g.txt test_files/a.txt:test_files/g.txt
	echo "\n\n-------Comparing and deleting Small files-------" >> raport3b.txt
	./main create_table 10 compare_pairs test_files/d.txt:test_files/b.txt test_files/d.txt:test_files/e.txt test_files/e.txt:test_files/b.txt remove_block 0 remove_block 1 remove_block 2
	echo "\n\n-------Comparing and deleting medium files-------" >> raport3b.txt
	./main create_table 10 compare_pairs test_files/c.txt:test_files/f.txt test_files/c.txt:test_files/g.txt test_files/f.txt:test_files/g.txt remove_block 0 remove_block 1 remove_block 2
	echo "\n\n-------Comparing and deleting big files-------" >> raport3b.txt
	./main create_table 10 compare_pairs test_files/a.txt:test_files/h.txt test_files/h.txt:test_files/g.txt test_files/a.txt:test_files/g.txt remove_block 0 remove_block 1 remove_block 2
	echo "\n\n-------Comparing and deleting operations Small files-------" >> raport3b.txt
	./main create_table 10 compare_pairs test_files/d.txt:test_files/b.txt test_files/d.txt:test_files/e.txt test_files/e.txt:test_files/b.txt remove_operation 0 0 remove_operation 0 1 remove_operation 1 0 remove_operation 2 0 remove_operation 1 1 remove_operation 2 1
	echo "\n\n-------Comparing and deleting operations medium files-------" >> raport3b.txt
	./main create_table 10 compare_pairs test_files/c.txt:test_files/f.txt test_files/c.txt:test_files/g.txt test_files/f.txt:test_files/g.txt remove_operation 0 0 remove_operation 0 1 remove_operation 1 0 remove_operation 2 0 remove_operation 1 1 remove_operation 2 1
	echo "\n\n-------Comparing and deleting operations big files-------" >> raport3b.txt
	./main create_table 10 compare_pairs test_files/a.txt:test_files/h.txt test_files/h.txt:test_files/g.txt test_files/a.txt:test_files/g.txt remove_operation 0 0 remove_operation 0 1 remove_operation 1 0 remove_operation 2 0 remove_operation 1 1 remove_operation 2 1
	

test_all:
	echo "#######################O1#################################\n" > raport3b.txt
	printf "************************static****************************\n" >> raport3b.txt
	make static
	make test
	printf "\n***********************shared*****************************\n" >> raport3b.txt
	make shared
	make test
	printf "\n***********************dynamic*****************************\n" >> raport3b.txt
	make dynamic
	make test
	printf "\n#######################O2#################################\n" >> raport3b.txt
	printf "************************static****************************\n" >> raport3b.txt
	make static
	make test
	printf "\n***********************shared*****************************\n" >> raport3b.txt
	make shared
	make test
	printf "\n***********************dynamic*****************************\n" >> raport3b.txt
	make dynamic
	make test
	printf "\n#######################O3#################################\n" >> raport3b.txt
	printf "************************static****************************\n" >> raport3b.txt
	make static
	make test
	printf "\n***********************shared*****************************\n" >> raport3b.txt
	make shared
	make test
	printf "\n***********************dynamic*****************************\n" >> raport3b.txt
	make dynamic
	make test