cmake_minimum_required(VERSION 3.22)
project (Server)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -Wextra -std=c++20 -pthread")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE debug)
set(SERVER_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/include)
include_directories(${SERVER_SRC_INCLUDE_DIR})
add_subdirectory(${PROJECT_SOURCE_DIR}/src/lib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

add_executable(server src/server.cpp)
add_executable(client src/client.cpp)
add_executable(test src/test.cc)
target_link_libraries(server LIB)
target_link_libraries(client LIB)
target_link_libraries(test LIB)