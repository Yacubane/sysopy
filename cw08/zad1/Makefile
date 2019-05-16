CC = gcc
CFLAGS = -Wall

.PHONY: clean

main : main.c image.c filter.c errors.c utils.c
	$(CC) $(CFLAGS) main.c image.c filter.c errors.c utils.c -o main -pthread

run : main
	./main

clean :
	rm -f trucker
	rm -f loader
	rm -f spawner
