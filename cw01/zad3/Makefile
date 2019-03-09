CC=gcc
CFLAGS=-Wall

all: 

static : ../zad2/test.c 
	$(CC) $(CFLAGS) $(ADDITIONALCFLAGS) ../zad2/test.c ../zad2/reporting.c ../zad1/libfinder.a -I../zad1/ -o test

shared : ../zad2/test.c
	$(CC) $(CFLAGS) $(ADDITIONALCFLAGS) ../zad2/test.c ../zad2/reporting.c -L../zad1/ -I../zad1/ -lfinder -o test

dynamic : ../zad2/test.c 
	$(CC) $(CFLAGS) $(ADDITIONALCFLAGS) ../zad2/test.c ../zad2/reporting.c ../zad2/finder_dynamic.c -ldl -D DYNAMIC -o test

test_static : static
	./test create_table 100 start_reporting report_static.txt $$(cat test.txt)

test_shared : shared
	./test create_table 100 start_reporting report_shared.txt $$(cat test.txt)

test_dynamic : dynamic
	./test create_table 100 start_reporting report_dynamic.txt $$(cat test.txt)

test: test_dynamic test_shared test_static

test_staticO1 : stat

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so*