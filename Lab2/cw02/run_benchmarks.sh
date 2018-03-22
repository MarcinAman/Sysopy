#!/bin/bash
make clean
make compile
echo "Test szybkosci wyszukiwania dla wszystkich katalogow domowych" >> wyniki.txt
./main ~ "<" 2018 3 22 2 >> wyniki.txt
echo "Test dla wszystkich plikow w komputerze " >> wyniki.txt
./main / "<" 2018 3 22 2 >> wyniki.txt
make clean
