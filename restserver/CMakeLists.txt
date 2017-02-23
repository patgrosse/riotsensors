cmake_minimum_required(VERSION 3.0)

project(riotsensors_server)

# dependencies
include_directories(${LIB_DIR}/pistache/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/../linuxconnector/include)

# source files
file(GLOB C_FILES *.cpp)
file(GLOB H_FILES include/*.h)
set(SOURCE_FILES ${C_FILES} ${H_FILES})

# compiler flags etc
set(CMAKE_CXX_STANDARD 11)

# targets
add_executable(riotsensors_server ${SOURCE_FILES})
target_link_libraries(riotsensors_server net_static)
target_link_libraries(riotsensors_server riotsensors_linux)
