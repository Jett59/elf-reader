cmake_minimum_required(VERSION 3.15)

project(elf-reader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(elf src/reader.cpp)
