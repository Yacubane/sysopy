CC = gcc
CFLAGS = -Wall

.PHONY: test_lazy test_buffer

all : main_dir main_nftw
default : all

monitor : monitor.c file_monitor.c
	$(CC) $(CFLAGS) monitor.c file_monitor.c -o monitor

tester : ../zad2/tester.c
	$(CC) $(CFLAGS) ../zad2/tester.c -o tester


test_files : 
	rm -rf test_files
	mkdir test_files
	mkdir test_files/buffer
	mkdir test_files/lazy

	echo 'Hello, sysops.' > test_files/buffer/file1.txt
	echo 'Hello, sysops.' > test_files/buffer/file2.txt
	cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 104857600 | head -n 1 > test_files/buffer/file3.txt

	cp test_files/buffer/file1.txt test_files/lazy/file1.txt
	cp test_files/buffer/file2.txt test_files/lazy/file2.txt
	cp test_files/buffer/file3.txt test_files/lazy/file3.txt

test_buffer : test_files tester monitor
	./tester test_files/buffer/file1.txt 1 5 10 &
	./tester test_files/buffer/file2.txt 5 10 20 &
	./tester test_files/buffer/file3.txt 10 15 25 &

	#./monitor list_buffer.txt 30 buffer 1 100
	./monitor list_buffer.txt 30 buffer 1 200

	pkill tester

test_lazy : test_files tester monitor
	./tester test_files/lazy/file1.txt 1 5 10 &
	./tester test_files/lazy/file2.txt 5 10 20 &
	./tester test_files/lazy/file3.txt 10 15 25 &

	#./monitor list_lazy.txt 30 lazy 1 1
	./monitor list_lazy.txt 30 lazy 1 200

	pkill tester


clean :
	rm -rf archiwum
	rm -f tester
	rm -f monitor
	rm -rf test_files