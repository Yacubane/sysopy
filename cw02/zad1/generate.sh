#!/usr/bin/env bash
rm -f wyniki_gen.txt
touch wyniki_gen.txt

record_size=(1 1 4 4 512 512 1024 1024 4096 4096 8192 8192)
record_num=(2000 2000 2000 2000 2000 2000 2000 2000 2000 2000 2000 2000)
for index in ${!record_size[*]}
do
    ./main generate data ${record_num[$index]} ${record_size[$index]} > tmp
    head -n 1 tmp >> wyniki_gen.txt

    printf "Test ${record_num[$index]} rekordów o rozmiarze ${record_size[$index]}\n" >> wyniki_gen.txt
    printf "Test kopiowania - funkcje systemowe\n" >> wyniki_gen.txt
    ./main copy data data2 ${record_num[$index]} ${record_size[$index]} sys > tmp
    tail -n 1 tmp >> wyniki_gen.txt

    printf "Test kopiowania - biblioteka C\n" >> wyniki_gen.txt
    ./main copy data data3 ${record_num[$index]} ${record_size[$index]} lib > tmp
    tail -n 1 tmp >> wyniki_gen.txt

    printf "Test sortowania - funkcje systemowe\n" >> wyniki_gen.txt
    ./main sort data2 ${record_num[$index]} ${record_size[$index]} sys > tmp
    tail -n 1 tmp >> wyniki_gen.txt

    printf "Test sortowania - biblioteka C\n" >> wyniki_gen.txt
    ./main sort data3 ${record_num[$index]} ${record_size[$index]} lib > tmp
    tail -n 1 tmp >> wyniki_gen.txt

    printf "\n" >> wyniki_gen.txt
done