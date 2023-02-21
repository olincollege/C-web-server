cmake_minimum_required(VERSION 3.24)
project(C_web_server C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(C_web_server
        src/main.c src/connection.h src/connection.c)
