cmake_minimum_required(VERSION 3.5)

project(ampl2omt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include(FetchContent)

FetchContent_Declare(
        ampl_mp
        GIT_REPOSITORY https://github.com/ampl/mp.git
        GIT_TAG develop
)

FetchContent_MakeAvailable(ampl_mp)
# MP options
set(BUILD_EXAMPLES OFF)
set(SKIP_BUILD_MP ON)

include_directories(${PROJECT_SOURCE_DIR}/include ${ampl_mp_SOURCE_DIR}/include)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)
