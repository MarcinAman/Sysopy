#!/bin/bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
make dynamic lvl=$1
./main create_table_s 1000 800
./main create_table_d 1000 800
./main search_element_s_test 100 2000 800
./main search_elemet_d_test 100 2000 800
./main rm_add_block_s_test 100 2000 800
./main rm_add_block_d_test 100 2000 800
./main rm_add_number_s_test 100 2000 800
./main rm_add_number_d_test 100 2000 8
make clean
