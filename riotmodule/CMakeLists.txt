# =============================================================================
# project should not be compiled with cmake, use RIOT OS's make structure instead
# CMake is just here for IDEs
# =============================================================================

cmake_minimum_required(VERSION 3.0)

project(riotsensors_riotmodule)

# directories
set(RIOT_DIR ${LIB_DIR}/RIOT)

# dependencies
include_directories(${LIB_DIR}/libspt/src/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../packets/include)

# source files
file(GLOB C_FILES *.c)
file(GLOB H_FILES include/*.h)
set(SOURCE_FILES ${C_FILES} ${H_FILES})

# tests
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/tests)

# compiler flags etc
find_program(arm-none-eabi-gcc arm-none-eabi-gcc)
if (${arm-none-eabi-gcc} STREQUAL "arm-none-eabi-gcc-NOTFOUND")
    message("Using downloaded arm-none-eabi-gcc")
    set(CMAKE_C_COMPILER "${LIB_DIR}/gcc-arm-none-eabi-6_2-2016q4/bin/arm-none-eabi-gcc")
else ()
    message("Using pre-installed arm-none-eabi-gcc")
    set(CMAKE_C_COMPILER ${arm-none-eabi-gcc})
endif ()
set(CMAKE_C_STANDARD 99)

# RIOT OS includes
include_directories(${RIOT_DIR}/sys/posix/pthread/include)
include_directories(${RIOT_DIR}/core/include)
include_directories(${RIOT_DIR}/sys/include)
include_directories(${RIOT_DIR}/drivers/include)
include_directories(${RIOT_DIR}/cpu/lpc2387/include)

# targets
add_library(riotsensors_riot ${SOURCE_FILES})
target_link_libraries(riotsensors_riot riotsensors_packets)
target_link_libraries(riotsensors_riot libspt)
