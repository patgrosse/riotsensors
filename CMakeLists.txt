# =============================================================================
# CMake is mainly here for IDEs and unit tests
# =============================================================================

cmake_minimum_required(VERSION 3.0)

# directories
set(LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/lib)
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# googletest library
add_subdirectory(${LIB_DIR}/googletest/googletest)

# pistache http server
add_subdirectory(${LIB_DIR}/pistache)

# libspt
add_subdirectory(${LIB_DIR}/libspt)

# riotsensors riotmodule and linuxconnector
project(riotsensors)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/packets)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/riotmodule)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/linuxconnector)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/restserver)
