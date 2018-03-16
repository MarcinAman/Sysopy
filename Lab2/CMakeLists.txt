cmake_minimum_required(VERSION 3.9)
project(Lab2 C)

set(CMAKE_C_STANDARD 99)

add_executable(Lab2 cw01/main.c cw01/SysOperation.c cw01/SysOperation.h cw01/LibOperation.c cw01/LibOperation.h)