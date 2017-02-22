# =============================================================================
# CMake is mainly here for IDEs and unit tests
# =============================================================================

cmake_minimum_required(VERSION 3.0)

# directories
set(LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/lib)

# googletest library
add_subdirectory(${LIB_DIR}/googletest/googletest)

# riotsensors riotmodule and linuxconnector
project(riotsensors)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/riotmodule)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/linuxconnector)
