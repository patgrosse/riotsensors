cmake_minimum_required(VERSION 3.0)

project(riotsensors_server)

# cmake inclusions
include(${CMAKE_CURRENT_LIST_DIR}/../cmake/FindLibEvent.cmake)

# dependencies
include_directories(${LIB_DIR}/pistache/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../protocol/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../linuxconnector/include)
include_directories(${LIB_DIR}/rapidjson/include)
include_directories(${LIB_DIR}/libspt/src/include)
find_package(Threads)
link_directories(/usr/local/lib)

# source files
file(GLOB C_FILES *.cpp)
file(GLOB H_FILES include/*.h)
set(SOURCE_FILES ${C_FILES} ${H_FILES})

# compiler flags etc
set(CMAKE_CXX_STANDARD 11)
use_c99()
set(CMAKE_CXX_FLAGS "-O0 -DWITH_POSIX")

# targets
add_executable(riotsensors_server ${SOURCE_FILES})
target_link_libraries(riotsensors_server net_static)
target_link_libraries(riotsensors_server riotsensors_linux)
target_link_libraries(riotsensors_server riotsensors_protocol)
target_link_libraries(riotsensors_server libspt)
target_link_libraries(riotsensors_server coap-1)
target_link_libraries (riotsensors_server ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(riotsensors_server ${LIBEVENT_LIB})
