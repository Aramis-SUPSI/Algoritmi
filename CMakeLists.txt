cmake_minimum_required(VERSION 3.24)
project(Algoritmi)

set(CMAKE_CXX_STANDARD 14)

include_directories(mmkp_verifier)

add_executable(Algoritmi
        mmkp_verifier/data.cpp
        mmkp_verifier/data.h
        mmkp_verifier/main.cpp)
