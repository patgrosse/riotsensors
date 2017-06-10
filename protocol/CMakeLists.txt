cmake_minimum_required(VERSION 3.0)

project(riotsensors_protocol)

# dependencies
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

# source files
file(GLOB C_FILES *.c)
file(GLOB H_FILES include/*.h)
set(SOURCE_FILES ${C_FILES} ${H_FILES})

# compiler flags etc
set(CMAKE_C_FLAGS "-Werror -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g -Wundef -Wshadow -Wcast-align -Wstrict-overflow=5 -Wno-write-strings \
-Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wunreachable-code")
use_c99()

# targets
add_library(riotsensors_protocol ${SOURCE_FILES})

# tests
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/tests)
