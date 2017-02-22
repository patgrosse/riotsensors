cmake_minimum_required(VERSION 3.0)
project(riotsensors_tests)

# directories
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
set(LIB_DIR ${SRC_DIR}/lib)

add_subdirectory(${LIB_DIR}/googletest/googletest ${CMAKE_CURRENT_BINARY_DIR}/googletest)

# compiler flags
set(CMAKE_C_FLAGS "-Werror -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g -Wundef -Wshadow -Wcast-align -Wstrict-overflow=5 -Wno-write-strings \
-Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wunreachable-code")
set(CMAKE_C_STANDARD 99)

# dependencies
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${LIB_DIR}/libspt/src/include)
include_directories(${SRC_DIR}/include)

# sources
set(FILES_IN_TEST ${SRC_DIR}/rs.c ${SRC_DIR}/ieee754_network.c)
set(TEST_FILES rs_test.cpp ieee754_network_test.cpp)

# targets
add_executable(runTests ${FILES_IN_TEST} ${TEST_FILES})
target_link_libraries(runTests gtest gtest_main)