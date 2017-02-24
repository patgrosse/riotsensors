cmake_minimum_required(VERSION 3.0)
project(riotsensors_linuxconnector_tests)

# directories
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

# dependencies
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${SRC_DIR}/include)

# sources
set(FILES_IN_TEST ${SRC_DIR}/rs_connector.c)
set(TEST_FILES "")

# targets
add_executable(linux_tests ${FILES_IN_TEST} ${TEST_FILES})
target_link_libraries(linux_tests gtest gtest_main)
target_link_libraries(linux_tests riotsensors_packets)
target_link_libraries(linux_tests libspt)
