CC = gcc -Wall -std=c11 -ggdb
LIBRARY_PATH = ../zad1


static:
	$(CC) main.c -o main -I$(LIBRARY_PATH) $(LIBRARY_PATH)/library.c


clean:
	rm -f *.o

test:
	make static
	rm -f raport2.txt
	echo "\n\n-------Creating big array-------" >> raport2.txt
	./main create_table 1000
	echo "\n\n-------Comparing Small files-------" >> raport2.txt
	./main create_table 10 compare_pairs test_files/d.txt:test_files/b.txt test_files/d.txt:test_files/e.txt test_files/e.txt:test_files/b.txt
	echo "\n\n-------Comparing medium files-------" >> raport2.txt
	./main create_table 10 compare_pairs test_files/c.txt:test_files/f.txt test_files/c.txt:test_files/g.txt test_files/f.txt:test_files/g.txt
	echo "\n\n-------Comparing big files-------" >> raport2.txt
	./main create_table 10 compare_pairs test_files/a.txt:test_files/h.txt test_files/h.txt:test_files/g.txt test_files/a.txt:test_files/g.txt
	echo "\n\n-------Comparing and deleting Small files-------" >> raport2.txt
	./main create_table 10 compare_pairs test_files/d.txt:test_files/b.txt test_files/d.txt:test_files/e.txt test_files/e.txt:test_files/b.txt remove_block 0 remove_block 1 remove_block 2
	echo "\n\n-------Comparing and deleting medium files-------" >> raport2.txt
	./main create_table 10 compare_pairs test_files/c.txt:test_files/f.txt test_files/c.txt:test_files/g.txt test_files/f.txt:test_files/g.txt remove_block 0 remove_block 1 remove_block 2
	echo "\n\n-------Comparing and deleting big files-------" >> raport2.txt
	./main create_table 10 compare_pairs test_files/a.txt:test_files/h.txt test_files/h.txt:test_files/g.txt test_files/a.txt:test_files/g.txt remove_block 0 remove_block 1 remove_block 2
	echo "\n\n-------Comparing and deleting operations Small files-------" >> raport2.txt
	./main create_table 10 compare_pairs test_files/d.txt:test_files/b.txt test_files/d.txt:test_files/e.txt test_files/e.txt:test_files/b.txt remove_operation 0 0 remove_operation 0 1 remove_operation 1 0 remove_operation 2 0 remove_operation 1 1 remove_operation 2 1
	echo "\n\n-------Comparing and deleting operations medium files-------" >> raport2.txt
	./main create_table 10 compare_pairs test_files/c.txt:test_files/f.txt test_files/c.txt:test_files/g.txt test_files/f.txt:test_files/g.txt remove_operation 0 0 remove_operation 0 1 remove_operation 1 0 remove_operation 2 0 remove_operation 1 1 remove_operation 2 1
	echo "\n\n-------Comparing and deleting operations big files-------" >> raport2.txt
	./main create_table 10 compare_pairs test_files/a.txt:test_files/h.txt test_files/h.txt:test_files/g.txt test_files/a.txt:test_files/g.txt remove_operation 0 0 remove_operation 0 1 remove_operation 1 0 remove_operation 2 0 remove_operation 1 1 remove_operation 2 1
	


