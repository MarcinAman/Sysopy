#!/bin/bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
make dynamic lvl=3
./main init_static 1000 800
./main init_dynamic 1000 800
./main search_static 100 2000 800
./main search_dynamic 100 2000 800
./main rm_add_block_static 100 2000 800
./main rm_add_block_dynamic 100 2000 800
./main rm_add_rand_static 100 2000 800
./main rm_add_rand_dynamic 100 2000 800
make clean
