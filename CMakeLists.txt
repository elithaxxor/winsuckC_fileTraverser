cmake_minimum_required(VERSION 3.28)
project(winsuckC_fileTraverser C)

set(CMAKE_C_STANDARD 11)

add_library(winsuckC_fileTraverser STATIC library.c)
