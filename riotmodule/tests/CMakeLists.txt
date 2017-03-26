cmake_minimum_required(VERSION 3.0)
project(riotsensors_riotmodule_tests)

# directories
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

# compiler flags
set(CMAKE_C_FLAGS "-Werror -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -g -Wundef -Wshadow -Wcast-align -Wstrict-overflow=5 -Wno-write-strings \
-Waggregate-return -Wcast-qual -Wswitch-default -Wswitch-enum -Wunreachable-code")
set(CMAKE_C_STANDARD 99)
add_definitions("-DRS_NO_COAP")

# dependencies
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${LIB_DIR}/libspt/src/include)
include_directories(${SRC_DIR}/include)

# sources
set(FILES_IN_TEST ${SRC_DIR}/rs.c ${SRC_DIR}/rs_coap_server.c ${SRC_DIR}/rs_coap_utils.c)
set(TEST_FILES rs_test.cpp rs_coap_utils_test.cpp)

# targets
add_executable(riot_tests ${FILES_IN_TEST} ${TEST_FILES})
target_link_libraries(riot_tests gtest gtest_main)
target_link_libraries(riot_tests riotsensors_protocol)
target_link_libraries(riot_tests libspt)
