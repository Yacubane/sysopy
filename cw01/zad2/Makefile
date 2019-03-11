CC=gcc
CFLAGS=-Wall

all : test_static
default : all

main_static : main.c reporting.c ../zad1/libfinder.a
	$(CC) $(CFLAGS) main.c reporting.c ../zad1/libfinder.a -I../zad1/ -o main_static

main_shared : main.c reporting.c ../zad1/libfinder.so
	$(CC) $(CFLAGS) main.c reporting.c -L../zad1/ -I../zad1/ -lfinder -o main_shared

main_dynamic : main.c reporting.c ../zad1/libfinder.so
	$(CC) $(CFLAGS) main.c reporting.c finder_dynamic.c -ldl -D DYNAMIC -o main_dynamic

test_static : main_static
	./main_static $$(cat args.txt)

test_shared : main_shared
	./main_shared $$(cat args.txt)

test_dynamic : main_dynamic
	./main_dynamic $$(cat args.txt)

clean :
	rm -f main_*
	rm -f tmp

clean_all : clean
	rm -f raport2.txt