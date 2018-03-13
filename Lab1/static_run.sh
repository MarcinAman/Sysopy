#!/bin/bash
make main_static lvl=3
./static_main init_static 1000 800
./static_main init_dynamic 1000 800
./static_main search_static 100 2000 800
./static_main search_dynamic 100 2000 800
./static_main rm_add_block_static 100 2000 800
./static_main rm_add_block_dynamic 100 2000 800
./static_main rm_add_rand_static 100 2000 800
./static_main rm_add_rand_dynamic 100 2000 800
make clean
