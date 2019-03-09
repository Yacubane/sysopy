CC=gcc
CFLAGS=-Wall

all: static_lib, shared_lib

static_lib : finder.c 
	$(CC) $(CFLAGS) -c finder.c
	ar rcs libfinder.a finder.o

shared_lib : finder.c
	$(CC) $(CFLAGS) -fPIC -c finder.c
	$(CC) $(CFLAGS) -shared -Wl,-soname,libfinder.so.1 -o libfinder.so.1.0.0 finder.o -lc
	ln -s libfinder.so.1.0.0 libfinder.so.1
	ln -s libfinder.so.1.0.0 libfinder.so

add_shared_lib : shared_lib
	sudo cp libfinder.so.1.0.0 /usr/lib
	sudo rm -f /usr/lib/libfinder.so.1
	sudo rm -f /usr/lib/libfinder.so
	sudo ln -s /usr/lib/libfinder.so.1.0.0 /usr/lib/libfinder.so.1
	sudo ln -s /usr/lib/libfinder.so.1.0.0 /usr/lib/libfinder.so
	sudo ldconfig

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so*
	sudo rm -f /usr/lib/libfinder*