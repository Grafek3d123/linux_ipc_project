cmake_minimum_required(VERSION 3.25)
project(LinuxIPCProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#add_executable(client src/main.cpp src/client.cpp)
add_executable(client src/client.cpp)
add_executable(server src/server.cpp)

include_directories(${CMAKE_SOURCE_DIR}/src)

target_link_libraries(client pthread)
target_link_libraries(server pthread)
