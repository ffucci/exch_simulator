cmake_minimum_required(VERSION 3.24.1)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_COMPILER /usr/local/opt/llvm/bin/clang++)
set(CMAKE_C_COMPILER /usr/local/opt/llvm/bin/clang)
set(CMAKE_CXX_STANDARD 20)

project(asio)

add_subdirectory(lib)
add_subdirectory(apps)