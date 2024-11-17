# CMakeLists.txt for memorypool project

cmake_minimum_required(VERSION 2.8.1)

project(zmemorypool)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

add_subdirectory("zMemoryPool")

add_subdirectory("MemoryPoolTest")