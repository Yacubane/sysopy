#!/usr/bin/env bash
rm -rf test_files
mkdir test_files
mkdir test_files/home
mkdir test_files/usr
mkdir test_files/usr/lib
touch -t 201401011200 test_files/usr/lib/libfinder-2.0.so.0
touch -t 201201011200 test_files/usr/lib/libgdx-1.0.so.0 
mkdir test_files/home/lib/
touch -t 201601011200 test_files/home/lib/libdroid-1.2.so.0
ln -s ../../../test_files/home/lib/libdroid-1.2.so.0 test_files/usr/lib/libdroid-1.2.so.0
mkdir test_files/home/documents/
touch test_files/home/documents/homework.txt
echo 'Hello, sysops.' > test_files/home/documents/hello_world.txt
echo 'Stay Hungry. Stay Foolish.' > test_files/home/documents/quotes.txt
echo 'To be is to be perceived.' > test_files/home/documents/quotes2.txt
touch -t 201801011200 test_files/home/documents/java.deb
mkdir test_files/home/desktop