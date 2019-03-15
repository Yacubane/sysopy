CC = gcc
CFLAGS = -Wall

all : main_dir main_nftw
default : all

main_dir : main.c files_dir.c
	$(CC) $(CFLAGS) main.c files_dir.c -o main_dir

main_nftw : main.c files_nftw.c
	$(CC) $(CFLAGS) main.c files_nftw.c -o main_nftw

test : main_dir main_nftw
	./main_dir $$(cat args.txt)
	./main_nftw $$(cat args.txt)

clean :
	rm -f main_*