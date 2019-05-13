CC = gcc
CFLAGS = -Wall

.PHONY: test clean

all : test
default : all

trucker : box.c cirque_memblock.c conveyor_belt.c errors.c trucker.c utils.c sysv.c colors.c
	$(CC) $(CFLAGS) box.c cirque_memblock.c conveyor_belt.c errors.c trucker.c utils.c sysv.c colors.c -o trucker -lrt


loader : box.c cirque_memblock.c conveyor_belt.c errors.c loader.c utils.c sysv.c colors.c
	$(CC) $(CFLAGS) box.c cirque_memblock.c conveyor_belt.c errors.c loader.c utils.c sysv.c colors.c -o loader -lrt

loaders_spawner : loader errors.c utils.c loaders_spawner.c
	$(CC) $(CFLAGS) errors.c utils.c loaders_spawner.c -o spawner -lrt

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
