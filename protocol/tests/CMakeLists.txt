cmake_minimum_required(VERSION 3.0)
project(riotsensors_protocol_tests)

# directories
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

# compiler flags
set(CMAKE_C_FLAGS "-Werror -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g -Wundef -Wshadow -Wcast-align -Wstrict-overflow=5 -Wno-write-strings \
-Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wunreachable-code")
set(CMAKE_C_STANDARD 99)

# dependencies
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${SRC_DIR}/include)

# sources
set(FILES_IN_TEST ${SRC_DIR}/ieee754_network.c)
set(TEST_FILES ieee754_network_test.cpp)

# targets
add_executable(protocol_tests ${FILES_IN_TEST} ${TEST_FILES})
target_link_libraries(protocol_tests gtest gtest_main)