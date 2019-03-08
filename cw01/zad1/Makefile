all: 

static_lib : finder.c 
	gcc -c finder.c -Wall
	ar rcs libfinder.a finder.o

shared_lib : finder.c
	gcc -fPIC -c finder.c
	gcc -shared -Wl,-soname,libfinder.so.1 -o libfinder.so.1.0.1 finder.o -lc
	ln -s libfinder.so.1.0.1 libfinder.so.1
	ln -s libfinder.so.1.0.1 libfinder.so

add_shared_lib : shared_finder
	sudo cp libfinder.so.1.0.1 /usr/lib
	sudo rm -f /usr/lib/libfinder.so.1
	sudo rm -f /usr/lib/libfinder.so
	sudo ln -s /usr/libfinder.so.1.0.1 /usr/libfinder.so.1
	sudo ln -s /usr/libfinder.so.1.0.1 /usr/libfinder.so
	sudo ldconfig

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so*