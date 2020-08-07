
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c11 -g

src = $(wildcard *.c)
obj = $(src:.c=.o)

size1 = 10000
size2 = 20000

run: main
	./main

main: $(obj)
	$(CC) $(CFLAGS) $^ -o $@
	
all: run_tests

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	rm -f -r $(obj) main tests

run_tests: main
	rm -f wyniki.txt
	rm -f -r tests
	mkdir -p tests
	echo "\nSize 1:\n" >> wyniki.txt

	echo "Records count: 1000\n" >> wyniki.txt
	./main generate tests/t1 1000 1

	echo "Library functions: \n" >> wyniki.txt
	./main copy tests/t1 tests/t1_1 1000 1 lib >> wyniki.txt
	./main sort tests/t1_1 1000 1 lib >> wyniki.txt
	echo "System functions:\n" >> wyniki.txt >> wyniki.txt
	./main copy tests/t1 tests/t1_2  1000 1 sys >> wyniki.txt
	./main sort tests/t1_2 1000 1 sys >> wyniki.txt

	echo "Records count: 2000\n" >> wyniki.txt
	./main generate tests/t2  2000 1

	echo "Library functions: \n" >> wyniki.txt
	./main copy tests/t2 tests/t2_1 2000 1 lib >> wyniki.txt
	./main sort tests/t2_1 2000 1 lib >> wyniki.txt
	echo "System functions:\n" >> wyniki.txt >> wyniki.txt
	./main copy tests/t2 tests/t2_2 2000 1 sys >> wyniki.txt
	./main sort tests/t2_2 2000 1 sys >> wyniki.txt


	echo "\nSize 4:\n" >> wyniki.txt

	echo "Records count: $(size1)\n" >> wyniki.txt
	./main generate tests/t3 $(size1) 4

	echo "Library functions: \n" >> wyniki.txt
	./main copy tests/t3 tests/t3_1 $(size1) 4 lib >> wyniki.txt
	./main sort tests/t3_1 $(size1) 4 lib >> wyniki.txt
	echo "System functions:\n" >> wyniki.txt >> wyniki.txt
	./main copy tests/t3 tests/t3_2 $(size1) 4 sys >> wyniki.txt
	./main sort tests/t3_2 $(size1) 4 sys >> wyniki.txt

	echo "Records count: $(size2)\n" >> wyniki.txt
	./main generate tests/t4 $(size2) 4

	echo "Library functions: \n" >> wyniki.txt
	./main copy tests/t4 tests/t4_1 $(size2) 4 lib >> wyniki.txt
	./main sort tests/t4_1 $(size2) 4 lib >> wyniki.txt
	echo "System functions:\n" >> wyniki.txt >> wyniki.txt
	./main copy tests/t4 tests/t4_2 $(size2) 4 sys >> wyniki.txt
	./main sort tests/t4_2 $(size2) 4 sys >> wyniki.txt


	echo "\nSize 512:\n" >> wyniki.txt

	echo "Records count: $(size1)\n" >> wyniki.txt
	./main generate tests/t5 $(size1) 512

	echo "Library functions: \n" >> wyniki.txt
	./main copy tests/t5 tests/t5_1 $(size1) 512 lib >> wyniki.txt
	./main sort tests/t5_1 $(size1) 512 lib >> wyniki.txt
	echo "System functions:\n" >> wyniki.txt >> wyniki.txt
	./main copy tests/t5 tests/t5_2 $(size1) 512 sys >> wyniki.txt
	./main sort tests/t5_2 $(size1) 512 sys >> wyniki.txt

	echo "Records count: $(size2)\n" >> wyniki.txt
	./main generate tests/t6 $(size2) 512

	echo "Library functions: \n" >> wyniki.txt
	./main copy tests/t6 tests/t6_1 $(size2) 512 lib >> wyniki.txt
	./main sort tests/t6_1 $(size2) 512 lib >> wyniki.txt
	echo "System functions:\n" >> wyniki.txt >> wyniki.txt
	./main copy tests/t6 tests/t6_2 $(size2) 512 sys >> wyniki.txt
	./main sort tests/t6_2 $(size2) 512 sys >> wyniki.txt


	echo "\nSize 1024:\n" >> wyniki.txt

	echo "Records count: $(size1)\n" >> wyniki.txt
	./main generate tests/t7 $(size1) 1024

	echo "Library functions: \n" >> wyniki.txt
	./main copy tests/t7 tests/t7_1 $(size1) 1024 lib >> wyniki.txt
	./main sort tests/t7_1 $(size1) 1024 lib >> wyniki.txt
	echo "System functions:\n" >> wyniki.txt >> wyniki.txt
	./main copy tests/t7 tests/t7_2 $(size1) 1024 sys >> wyniki.txt
	./main sort tests/t7_2 $(size1) 1024 sys >> wyniki.txt

	echo "Records count: $(size2)\n" >> wyniki.txt
	./main generate tests/t8 $(size2) 1024

	echo "Library functions: \n" >> wyniki.txt
	./main copy tests/t8 tests/t8_1 $(size2) 1024 lib >> wyniki.txt
	./main sort tests/t8_1 $(size2) 1024 lib >> wyniki.txt
	echo "System functions:\n" >> wyniki.txt >> wyniki.txt
	./main copy tests/t8 tests/t8_2 $(size2) 1024 sys >> wyniki.txt
	./main sort tests/t8_2 $(size2) 1024 sys >> wyniki.txt


	echo "\nSize 4096:\n" >> wyniki.txt

	echo "Records count: $(size1)\n" >> wyniki.txt
	./main generate tests/t9 $(size1) 4096

	echo "Library functions: \n" >> wyniki.txt
	./main copy tests/t9 tests/t9_1 $(size1) 4096 lib >> wyniki.txt
	./main sort tests/t9_1 $(size1) 4096 lib >> wyniki.txt
	echo "System functions:\n" >> wyniki.txt >> wyniki.txt
	./main copy tests/t9 tests/t9_2 $(size1) 4096 sys >> wyniki.txt
	./main sort tests/t9_2 $(size1) 4096 sys >> wyniki.txt

	echo "Records count: $(size2)\n" >> wyniki.txt
	./main generate tests/t10 $(size2) 4096

	echo "Library functions: \n" >> wyniki.txt
	./main copy tests/t10 tests/t10_1 $(size2) 4096 lib >> wyniki.txt
	./main sort tests/t10_1 $(size2) 4096 lib >> wyniki.txt
	echo "System functions:\n" >> wyniki.txt >> wyniki.txt
	./main copy tests/t10 tests/t10_1 $(size2) 4096 sys >> wyniki.txt
	./main sort tests/t10_1 $(size2) 4096 sys >> wyniki.txt


	echo "\nSize 8192:\n" >> wyniki.txt

	echo "Records count: $(size1)\n" >> wyniki.txt
	./main generate tests/t11 $(size1) 8192

	echo "Library functions: \n" >> wyniki.txt
	./main copy tests/t11 tests/t11_1 $(size1) 8192 lib >> wyniki.txt
	./main sort tests/t11_1 $(size1) 8192 lib >> wyniki.txt
	echo "System functions:\n" >> wyniki.txt >> wyniki.txt
	./main copy tests/t11 tests/t11_2 $(size1) 8192 sys >> wyniki.txt
	./main sort tests/t11_2 $(size1) 8192 sys >> wyniki.txt

	echo "Records count: $(size2)\n" >> wyniki.txt
	./main generate tests/t12 $(size2) 8192

	echo "Library functions: \n" >> wyniki.txt
	./main copy tests/t12 tests/t12_1 $(size2) 8192 lib >> wyniki.txt
	./main sort tests/t12_1 $(size2) 8192 lib >> wyniki.txt
	echo "System functions:\n" >> wyniki.txt >> wyniki.txt
	./main copy tests/t12 tests/t12_2 $(size2) 8192 sys >> wyniki.txt
	./main sort tests/t12_2 $(size2) 8192 sys >> wyniki.txt
