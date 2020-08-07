CC=gcc -Wall

all: compile generate_tests run_tests check_tests clean

compile: generate.c macierz.c test.c
	$(CC) -o generate generate.c
	$(CC) -o macierz macierz.c -lm
	$(CC) -o test test.c

generate_tests: generate
	mkdir tests
	./generate 10 50 70 0 10

run_tests: macierz
	./macierz tests_list 5 1 1 1 100

check_tests: test
	./test tests_list

clean:
	rm -rf tests generate macierz test tests_list