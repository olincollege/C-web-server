# Some compilers warn that the return value of cr_assert gets converted from an
# int to a unsigned char:1. To avoid this error in those compilers, turn off
# conversion warnings for test code.
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-conversion")

add_executable(test_parsing test_parsing.c)
target_link_libraries(test_parsing
        PRIVATE parsing
        PUBLIC ${CRITERION})
add_test(
        NAME test_parsing
        COMMAND test_parsing ${CRITERION_FLAGS}
)

add_executable(test_readfiles test_readfiles.c)
target_link_libraries(test_readfiles
        PRIVATE readfiles
        PUBLIC ${CRITERION})
add_test(
        NAME test_readfiles
        COMMAND test_readfiles ${CRITERION_FLAGS}
)
