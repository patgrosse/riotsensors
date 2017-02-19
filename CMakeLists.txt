# =============================================================================
# project should not be compiled with cmake, use RIOT OS's make structure instead
# CMake is just here for IDEs
# =============================================================================

cmake_minimum_required(VERSION 3.0)

project(riotsensors)

# directories
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set(RIOT_DIR ${BUILD_DIR}/RIOT)

# dependencies
include_directories(${BUILD_DIR}/libspt/src/include)
include_directories(${CMAKE_SOURCE_DIR}/include)

# source files
set(SOURCE_FILES dummy.c rs.c ieee754_network.c)

# tests
add_subdirectory(${CMAKE_SOURCE_DIR}/tests)

# compiler flags etc
set(CMAKE_C_COMPILER "${BUILD_DIR}/gcc-arm-none-eabi-6_2-2016q4/bin/arm-none-eabi-gcc")
set(CMAKE_C_STANDARD 99)

# RIOT OS includes
include_directories(${RIOT_DIR}/sys/posix/pthread/include)
include_directories(${RIOT_DIR}/core/include)
include_directories(${RIOT_DIR}/sys/include)
include_directories(${RIOT_DIR}/drivers/include)
include_directories(${RIOT_DIR}/cpu/lpc2387/include)

# targets
add_library(riotsensors ${SOURCE_FILES})
