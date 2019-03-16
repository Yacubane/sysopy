CC = gcc
CFLAGS = -Wall

.PHONY: test

all : main_dir main_nftw
default : all

main_dir : main.c files_dir.c
	$(CC) $(CFLAGS) main.c files_dir.c -o main_dir

main_nftw : main.c files_nftw.c
	$(CC) $(CFLAGS) main.c files_nftw.c -o main_nftw

test_files :
	./generate_files.sh

test : test_files main_dir main_nftw
	./test.sh
	
clean :
	rm -f main_*
	rm -rf test_files