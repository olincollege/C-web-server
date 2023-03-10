cmake_minimum_required(VERSION 3.2)
project(C_web_server C)

set(CMAKE_C_STANDARD 17)

set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -Wshadow -Wconversion -Wunreachable-code")

add_subdirectory(src)

add_executable(C_Web_Server src/main.c)

target_link_libraries(C_Web_Server connection handler parsing readfiles)

include(CTest)
find_library(CRITERION criterion REQUIRED)
set(CRITERION_FLAGS --verbose -j1)

add_subdirectory(test)
