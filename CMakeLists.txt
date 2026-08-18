cmake_minimum_required(VERSION 3.15)
project(OpenXLSXWrapper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(OpenXLSXWrapper SHARED wrapper.cpp)

target_include_directories(OpenXLSXWrapper PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX/headers
)

file(GLOB_RECURSE OPENXLSX_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/OpenXLSX/sources/*.cpp")
target_sources(OpenXLSXWrapper PRIVATE ${OPENXLSX_SRCS})
