CC = gcc
CFLAGS = -Wall

.PHONY: clean

main : main.c image.c filter.c errors.c utils.c gaussian_filter.c
	$(CC) $(CFLAGS) main.c image.c filter.c errors.c utils.c gaussian_filter.c -o main -lm -pthread

run : main
	./main

clean :
	rm -f main
