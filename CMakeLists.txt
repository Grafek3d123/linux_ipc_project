cmake_minimum_required(VERSION 3.25)
project(LinuxIPCProject)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_STANDART_REQUIRED True)

add_executable(client src/main.cpp src/client.cpp)
add_executable(server src/server.cpp)

include_directories(${CMAKE_SOURCE_DIR}/src)
