cmake_minimum_required(VERSION 3.0)
project(riotsensors_tests)

add_subdirectory(lib/googletest/googletest)

# compiler flags
set(CMAKE_C_FLAGS "-Werror -Wall -Wextra -std=gnu99")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g -Wundef -Wshadow -Wcast-align -Wstrict-overflow=5 -Wno-write-strings \
-Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wunreachable-code")
set(CMAKE_C_STANDARD 99)

# dependencies
include_directories(../include)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# sources
FILE(GLOB RS_SOURCES ../ieee754_network.c)

# targets
add_executable(runTests ieee754_network_test.cpp ${RS_SOURCES})
target_link_libraries(runTests gtest gtest_main)