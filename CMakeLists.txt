# =============================================================================
# CMake is mainly here for IDEs and unit tests
# =============================================================================

cmake_minimum_required(VERSION 3.0)

# directories
set(LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/lib)
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# googletest library
add_subdirectory(${LIB_DIR}/googletest/googletest)

# rapidjson library
add_subdirectory(${LIB_DIR}/rapidjson)

# pistache http server
include_directories(${LIB_DIR}/rapidjson/include)
add_subdirectory(${LIB_DIR}/pistache)

# libspt
add_subdirectory(${LIB_DIR}/libspt)

# riotsensors riotmodule and linuxconnector
project(riotsensors)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/protocol)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/riotmodule)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/linuxconnector)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/restserver)
