#CMakeList to make media_filter lib

cmake_minimum_required(VERSION 2.6)
project(zMemoryPool)
#The version number
set(memory_pool_VERSION_MAJOR 1)
set(memory_pool_VERSION_MINOR 0)

if(MSVC90)
    if(NOT DEFINED BOOST_DIR)
        message(FATAL_ERROR "Must define BOOST_DIR for boost root director when compile using vs2008")
    endif()
    set(BOOST_INC "${BOOST_DIR}/include")
    set(BOOST_LIB "${BOOST_DIR}/lib")
endif()

#configure a header file to pass some of the CMake settings
#to the source code
configure_file(
    "${PROJECT_SOURCE_DIR}/memory_pool_cfg.h.in"
    "${PROJECT_BINARY_DIR}/memory_pool_cfg.h"
    )

set(SRC_DIR src)
set(INC_DIR "include_pri" "include")

include_directories(
	"${PROJECT_BINARY_DIR}"
	${BOOST_INC}
    ${INC_DIR}
)

if(MSVC90)
    link_directories(
        "${BOOST_LIB}"
    )
endif()

if (MSVC)
    add_definitions(
        -DMEMORYPOOL_EXPORT
    )
    if(MSVC_VERSION LESS 1600)
        include_directories(
            "${PROJECT_SOURCE_DIR}/../vs2008_header"
        )
    endif()
endif()
#add_subdirectory(../test ../test)

set(SRC_LIST 
    include/MemoryPool.h
    include/MemoryPoolHeader.h
    include/MediaMemPool.h
    include_pri/FragmentBlockMemoryPool.h
    include_pri/FragmentBlockPool.h
    ${SRC_DIR}/FragmentBlockPool.cpp 
    ${SRC_DIR}/MediaMemPool.cpp
    ${SRC_DIR}/MemoryPool.cpp
    ${SRC_DIR}/FragmentBlockMemoryPool.cpp
)

add_library(memory_pool SHARED ${SRC_LIST})

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /Od")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
endif()

# using c++17 for shared_mutex
set_target_properties(memory_pool PROPERTIES CXX_STANDARD 17)
#目录结构
function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

assign_source_group(${SRC_LIST})

#target_link_libraries(memory_pool boost_thread boost_date_time)

install (TARGETS memory_pool DESTINATION bin)
install (FILES ./include/MediaMemPool.h 
    ./include/MemoryPool.h 
    ./include/MemoryPoolHeader.h 
    DESTINATION include)
