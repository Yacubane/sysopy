CC=gcc
CFLAGS=-Wall $(if $(O),-O$(O))
APPEND=0
RESULT_FILE=results.txt
RESULT_NAME=TEST

all: clean_all report3a report3b
default : all

main_static : ../zad2/main.c  ../zad2/reporting.c ../zad1/libfinder.a
	$(CC) $(CFLAGS) ../zad2/main.c ../zad2/reporting.c ../zad1/libfinder.a -I../zad1/ -o main_static

main_shared : ../zad2/main.c  ../zad2/reporting.c ../zad1/libfinder.so
	$(CC) $(CFLAGS) ../zad2/main.c ../zad2/reporting.c -L../zad1/ -I../zad1/ -lfinder -o main_shared

main_dynamic : ../zad2/main.c  ../zad2/reporting.c ../zad1/libfinder.so
	$(CC) $(CFLAGS) ../zad2/main.c ../zad2/reporting.c ../zad2/finder_dynamic.c -ldl -D DYNAMIC -o main_dynamic

test_static : main_static
	./main_static start_reporting $(RESULT_FILE) $(RESULT_NAME) $(APPEND) $$(cat args.txt)

test_shared : main_shared
	./main_shared start_reporting $(RESULT_FILE) $(RESULT_NAME) $(APPEND) $$(cat args.txt)

test_dynamic : main_dynamic
	./main_dynamic start_reporting $(RESULT_FILE) $(RESULT_NAME) $(APPEND) $$(cat args.txt)

report3b:
	rm -f results3b.txt
	for test in test_static test_shared test_dynamic ; do \
		for optimization in 0 1 2 3 s ; do \
    		make $$test APPEND=1 O=$$optimization RESULT_FILE=results3b.txt RESULT_NAME=$$test""_optimize-O""$$optimization""  ; \
		done ; \
	done

clean:
	rm -f main*
	rm -f tmp

clean_all : clean
	rm -f results3b.txt

