cmake_minimum_required(VERSION 3.28.3)
#Makes sure IDE understands the file paths
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(pegparser LANGUAGES CXX)

add_executable(pegparser src/pegparser.cpp)
target_compile_features(pegparser PUBLIC cxx_std_17)

add_subdirectory(extern/cpp-peglib)   # provides target: peglib
target_link_libraries(pegparser PUBLIC peglib)
