cmake_minimum_required(VERSION 3.6)
project(Deduplication_Project_P2)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c List.c Block.c Directory.c File.c HashTable.c HashTableF.c Utilities.c TextParsing.c TextParsing.h)
add_executable(Deduplication_Project_P2 ${SOURCE_FILES})