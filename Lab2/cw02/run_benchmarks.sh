#!/bin/bash
make clean > /dev/null 2>&1
make compile > /dev/null 2>&1
echo "Test szybkosci wyszukiwania dla wszystkich katalogow domowych" >> wyniki.txt
./main ~ "<" 2018 3 22 2 >> wyniki.txt
echo "Test dla wszystkich plikow w komputerze " >> wyniki.txt
./main / "<" 2018 3 22 2 >> wyniki.txt
make clean >> /dev/null 2>&1
