cmake_minimum_required(VERSION 3.13)  # CMake version check
project(Algoritmi)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 14)            # Enable c++14 standard

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES ../mmkp/MMKP_GR04/main.cpp ../mmkp/MMKP_GR04/data.h ../mmkp/MMKP_GR04/data.cpp)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(Algoritmi ${SOURCE_FILES})