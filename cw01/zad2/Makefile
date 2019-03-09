CC=gcc
CFLAGS=-Wall

all: 

static : test.c 
	$(CC) $(CFLAGS) test.c reporting.c ../zad1/libfinder.a -I../zad1/ -o test

shared : test.c
	$(CC) $(CFLAGS) test.c reporting.c -L../zad1/ -I../zad1/ -lfinder -o test

dynamic : test.c 
	$(CC) $(CFLAGS) test.c reporting.c finder_dynamic.c -ldl -D DYNAMIC -o test

test_static : static
	./test $$(cat test2.txt)

test_shared : shared
	./test $$(cat test2.txt)

test_dynamic : dynamic
	./test $$(cat test2.txt)

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so*