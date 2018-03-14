#!/bin/bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:`pwd`
make main_static lvl=$1
./static_main create_table_s 1000 800
./static_main create_table_d 1000 800
./static_main search_element_s_test 100 2000 800
./static_main search_element_d_test 100 2000 800
./static_main rm_add_block_s_test 100 2000 800
./static_main rm_add_block_d_test 100 2000 800
./static_main rm_add_number_s_test 100 2000 800
./static_main rm_add_number_d_test 100 2000 800
make clean
