# =============================================================================
# project should not be compiled with cmake, use RIOT OS's make structure instead
# CMake is just here for IDEs
# =============================================================================

cmake_minimum_required(VERSION 3.0)

project(riotsensors)

# directories
set(LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/lib)
set(RIOT_DIR ${LIB_DIR}/RIOT)

# dependencies
include_directories(${LIB_DIR}/libspt/src/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

# source files
file(GLOB C_FILES *.c)
file(GLOB H_FILES include/*.h)
set(SOURCE_FILES ${C_FILES} ${H_FILES})

# tests
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/tests)

# compiler flags etc
set(CMAKE_C_COMPILER "${LIB_DIR}/gcc-arm-none-eabi-6_2-2016q4/bin/arm-none-eabi-gcc")
set(CMAKE_C_STANDARD 99)

# RIOT OS includes
include_directories(${RIOT_DIR}/sys/posix/pthread/include)
include_directories(${RIOT_DIR}/core/include)
include_directories(${RIOT_DIR}/sys/include)
include_directories(${RIOT_DIR}/drivers/include)
include_directories(${RIOT_DIR}/cpu/lpc2387/include)

# targets
add_library(riotsensors ${SOURCE_FILES})
