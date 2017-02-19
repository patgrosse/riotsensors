cmake_minimum_required(VERSION 3.0)
project(riotsensors_tests)

# directories
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/../build)

add_subdirectory(lib/googletest/googletest)

# compiler flags
set(CMAKE_C_FLAGS "-Werror -Wall -Wextra -std=gnu99")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g -Wundef -Wshadow -Wcast-align -Wstrict-overflow=5 -Wno-write-strings \
-Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wunreachable-code")
set(CMAKE_C_STANDARD 99)

# dependencies
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${BUILD_DIR}/libspt/src/include)
include_directories(../include)

# sources
set(FILES_IN_TEST ../ieee754_network.c)
set(TEST_FILES ieee754_network_test.cpp)

# targets
add_executable(runTests ${FILES_IN_TEST} ${TEST_FILES})
target_link_libraries(runTests gtest gtest_main)