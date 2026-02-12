cmake_minimum_required(VERSION 3.28.3)
project(pegparser LANGUAGES CXX)

add_executable(pegparser ../src/pegparser.cpp)
target_compile_features(pegparser PRIVATE cxx_std_17)

add_subdirectory(extern/cpp-peglib)   # provides target: peglib
target_link_libraries(pegparser PRIVATE peglib)
