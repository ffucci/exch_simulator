cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_BUILD_TYPE "Release" CACHE
      STRING "Choose the type of build.")
set(CMAKE_CXX_STANDARD 20)

project(exch_simulator)

option(CODE_COVERAGE "Builds only the test executable." OFF)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

add_subdirectory(lib)
add_subdirectory(apps)