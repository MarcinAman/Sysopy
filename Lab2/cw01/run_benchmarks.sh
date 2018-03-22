#!/bin/bash
make clean
make compile
./main generate dane $1 $2 1
echo "Sorting took with sys:" >> wyniki.txt
./main sort dane $1 $2 1 >> wyniki.txt
rm -f dane
./main generate dane $1 $2 1
echo "Sorting took with lib:" >> wyniki.txt
./main sort dane $1 $2 1 >> wyniki.txt
echo "Coping took with sys:" >> wyniki.txt
echo "Buffer = 4" >> wyniki.txt
./main copy dane copy 4 0 >> wyniki.txt
rm -f copy
./main copy dane copy 4 1 >> wyniki.txt
echo "Buffer = 512" >> wyniki.txt
rm -f copy
./main copy dane copy 512 0 >> wyniki.txt
rm -f copy
./main copy dane copy 512 1 >> wyniki.txt
rm -f copy
echo "Buffer = 4096" >> wyniki.txt
./main copy dane copy 4096 0 >> wyniki.txt
rm -f copy
./main copy dane copy 4096 1 >> wyniki.txt
rm -f copy 
echo "Buffer = 8192" >> wyniki.txt
./main copy dane copy 8192 0 >> wyniki.txt
rm -f copy
./main copy dane copy 8192 1 >> wyniki.txt
rm -f copy
make clean
rm -f dane
