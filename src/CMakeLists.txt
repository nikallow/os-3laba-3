cmake_minimum_required(VERSION 4.3)
project(laba3 C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_executable(determinant main.c
        permutation.h
        permutation.c
        matrix.h
        matrix.c
        matrix_txt.h
        matrix_txt.c
        determinant.h
        determinant.c)

target_compile_definitions(determinant PRIVATE _POSIX_C_SOURCE=200809L)

target_compile_options(determinant PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wshadow
        -Wformat=2
        -Wstrict-prototypes
        -Wmissing-prototypes
        -Werror
)

target_link_libraries(determinant PRIVATE Threads::Threads)