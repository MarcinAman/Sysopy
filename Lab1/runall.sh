#!/bin/bash
rm -f raport_0.txt raport_1.txt raport_2.txt raport_3.txt
./static_run.sh 0 >> raport_0.txt
./static_run.sh 1 >> raport_1.txt
./static_run.sh 2 >> raport_2.txt
./static_run.sh 3 >> raport_3.txt
echo "\n Shared run: \n" >> raport_0.txt
./shared_run.sh 0 >> raport_0.txt
echo "\n Shared run: \n" >> raport_1.txt
./shared_run.sh 1 >> raport_1.txt
echo "\n Shared run: \n" >> raport_2.txt
./shared_run.sh 2 >> raport_2.txt
echo "\n Shared run: \n" >> raport_3.txt
./shared_run.sh 3 >> raport_3.txt
echo "\n Dynamic run: \n" >> raport_0.txt
./dynamic_run.sh 0 >> raport_0.txt
echo "\n Dynamic run: \n" >> raport_1.txt
./dynamic_run.sh 1 >> raport_1.txt
echo "\n Dynamic run: \n" >> raport_2.txt
./dynamic_run.sh 2 >> raport_2.txt
echo "\n Dynamic run: \n" >> raport_3.txt
./dynamic_run.sh 3 >> raport_3.txt
