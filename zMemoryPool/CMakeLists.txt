#CMakeList to make media_filter lib

cmake_minimum_required(VERSION 2.6)
project(zMemoryPool)
#The version number
set(memory_pool_VERSION_MAJOR 1)
set(memory_pool_VERSION_MINOR 0)

#configure a header file to pass some of the CMake settings
#to the source code
configure_file(
    "${PROJECT_SOURCE_DIR}/memory_pool_cfg.h.in"
    "${PROJECT_BINARY_DIR}/memory_pool_cfg.h"
    )

set(SRC_DIR ./src)
set(INC_DIR ./include_pri "./include")

include_directories("${PROJECT_BINARY_DIR}")
include_directories("./include_pri" "./include")

#add_subdirectory(../test ../test)

set(SRC_LIST 
    ${SRC_DIR}/FragmentBlockPool.cpp 
    ${SRC_DIR}/MediaMemPool.cpp
    ${SRC_DIR}/MemoryPool.cpp
    ${SRC_DIR}/FragmentBlockMemoryPool.cpp
)

add_library(memory_pool SHARED ${SRC_LIST})

install (TARGETS memory_pool DESTINATION bin)
install (FILES ./include/MediaMemPool.h 
    ./include/MemoryPool.h 
    ./include/MemoryPoolHeader.h 
    DESTINATION include)
