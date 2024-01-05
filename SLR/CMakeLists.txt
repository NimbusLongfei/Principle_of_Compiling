cmake_minimum_required(VERSION 3.26)
project(SLR)

set(CMAKE_CXX_STANDARD 17)

add_executable(SLR main.cpp
        SLR.cpp
        SLR.h
        utility.cpp
        lex.h)
