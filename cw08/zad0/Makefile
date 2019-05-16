CC = gcc
CFLAGS = -Wall

.PHONY: clean

main : main.c
	$(CC) $(CFLAGS) main.c -o main -pthread

run : main
	./main

clean :
	rm -f trucker
	rm -f loader
	rm -f spawner
