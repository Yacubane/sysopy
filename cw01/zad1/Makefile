CC=gcc
CFLAGS=-Wall

all: clean_all static_lib add_shared_lib
default : all

libfinder.a : finder.c 
	$(CC) $(CFLAGS) -c finder.c
	ar rcs libfinder.a finder.o

libfinder.so : finder.c
	$(CC) $(CFLAGS) -fPIC -c finder.c
	$(CC) $(CFLAGS) -shared -Wl,-soname,libfinder.so.1 -o libfinder.so.1.0.0 finder.o -lc
	ln -s libfinder.so.1.0.0 libfinder.so.1
	ln -s libfinder.so.1.0.0 libfinder.so

/usr/lib/libfinder.so : libfinder.so
	sudo cp libfinder.so.1.0.0 /usr/lib
	sudo rm -f /usr/lib/libfinder.so.1
	sudo rm -f /usr/lib/libfinder.so
	sudo ln -s /usr/lib/libfinder.so.1.0.0 /usr/lib/libfinder.so.1
	sudo ln -s /usr/lib/libfinder.so.1.0.0 /usr/lib/libfinder.so
	sudo ldconfig

static_lib : libfinder.a
shared_lib : libfinder.so
add_shared_lib : /usr/lib/libfinder.so

clean :
	rm -f *.o

clean_all : clean
	rm -f *.a
	rm -f *.so*
	sudo rm -f /usr/lib/libfinder*