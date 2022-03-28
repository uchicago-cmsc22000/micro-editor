cmake_minimum_required(VERSION 3.5.1)
project(micro C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

include_directories(src/)

add_executable(micro
    src/main.c
    src/terminal.c
    src/screen.c
    src/input.c
    src/row.c
    src/editor.c
    )
