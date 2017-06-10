# =============================================================================
# CMake is mainly here for IDEs and unit tests
# =============================================================================

cmake_minimum_required(VERSION 3.0)

macro(use_c99)
    if (CMAKE_VERSION VERSION_LESS "3.1")
        if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
            set (CMAKE_C_FLAGS "-std=gnu99 ${CMAKE_C_FLAGS}")
        endif ()
    else ()
        set (CMAKE_C_STANDARD 99)
    endif ()
endmacro(use_c99)

macro(use_cxx11)
    if (CMAKE_VERSION VERSION_LESS "3.1")
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set (CMAKE_CXX_FLAGS "-std=gnu++11 ${CMAKE_CXX_FLAGS}")
        endif ()
    else ()
        set (CMAKE_CXX_STANDARD 11)
    endif ()
endmacro(use_cxx11)

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
