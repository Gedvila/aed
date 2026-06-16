cmake_minimum_required(VERSION 4.2)
project(aed C)
set(CMAKE_C_STANDARD 11)

find_package(OpenMP REQUIRED)

add_executable(main main.c)
target_compile_options(main PRIVATE -O2)

target_link_libraries(main OpenMP::OpenMP_C m)