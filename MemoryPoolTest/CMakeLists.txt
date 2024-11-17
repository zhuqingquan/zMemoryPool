#CMakeList to make media_filter lib

cmake_minimum_required(VERSION 2.6)
project(MemoryPoolTest)
#The version number
#set(memory_pool_VERSION_MAJOR 1)
#set(memory_pool_VERSION_MINOR 0)

#configure a header file to pass some of the CMake settings
#to the source code
#configure_file(
#    "${PROJECT_SOURCE_DIR}/memory_pool_cfg.h.in"
#    "${PROJECT_BINARY_DIR}/memory_pool_cfg.h"
#    )

set(SRC_DIR .)
set(INC_DIR "${PROJECT_BINARY_DIR}/zMemoryPool/include")

if(MSVC90)
    if(NOT DEFINED BOOST_DIR)
        message(FATAL_ERROR "Must define BOOST_DIR for boost root director when compile using vs2008")
    endif()
    set(BOOST_INC "${BOOST_DIR}/include")
    set(BOOST_LIB "${BOOST_DIR}/lib")
    include_directories("${BOOST_INC}")
endif()

include_directories("${PROJECT_BINARY_DIR}")
include_directories("./" "../zMemoryPool/include_pri" "../zMemoryPool/include")

#add_subdirectory(../test ../test)

set(SRC_LIST 
    ${SRC_DIR}/MemoryPoolTest.cpp 
)

add_executable(MemoryPoolTest ${SRC_LIST})

target_link_libraries(MemoryPoolTest memory_pool)

#install (TARGETS memory_pool DESTINATION bin)
#install (FILES ./include/MediaMemPool.h 
#    ./include/MemoryPool.h 
#    ./include/MemoryPoolHeader.h 
#    DESTINATION include)
