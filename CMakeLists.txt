cmake_minimum_required(VERSION 3.28)
project(reverse_shell)

set(CMAKE_CXX_STANDARD 17)

add_executable(reverse_shell
        src/server.cpp
        src/client.cpp)

target_link_libraries(reverse_shell -lws2_32)