CC = gcc
CFLAGS = -Wall

.PHONY: test clean

all : test
default : all

trucker : ../zad1/box.c ../zad1/cirque_memblock.c ../zad1/conveyor_belt.c ../zad1/errors.c ../zad1/trucker.c ../zad1/utils.c posix.c ../zad1/colors.c
	$(CC) $(CFLAGS) ../zad1/box.c ../zad1/cirque_memblock.c ../zad1/conveyor_belt.c ../zad1/errors.c ../zad1/trucker.c ../zad1/utils.c posix.c ../zad1/colors.c -o trucker -lrt -pthread

loader : ../zad1/box.c ../zad1/cirque_memblock.c ../zad1/conveyor_belt.c ../zad1/errors.c ../zad1/loader.c ../zad1/utils.c posix.c ../zad1/colors.c
	$(CC) $(CFLAGS) ../zad1/box.c ../zad1/cirque_memblock.c ../zad1/conveyor_belt.c ../zad1/errors.c ../zad1/loader.c ../zad1/utils.c posix.c ../zad1/colors.c -o loader -lrt -pthread

loaders_spawner : loader ../zad1/errors.c ../zad1/utils.c ../zad1/loaders_spawner.c
	$(CC) $(CFLAGS) ../zad1/errors.c ../zad1/utils.c ../zad1/loaders_spawner.c -o spawner -lrt

run_trucker : trucker
	./trucker 20 5 10

run_loaders : loader
	./loader 1 &
	./loader 2 &
	./loader 3 &
	./loader 4 &
	./loader 5 &

run_spawner : loaders_spawner
	./spawner 5 5

clean :
	rm -f trucker
	rm -f loader
	rm -f spawner
