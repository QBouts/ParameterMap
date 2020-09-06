cmake_minimum_required(VERSION 3.11)
project(ParameterMap)

add_library(project_options INTERFACE)
IF(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang") 
target_compile_options(project_options INTERFACE "-std=c++2a")
ELSE()
target_compile_options(project_options INTERFACE "-std=c++17")
target_compile_options(project_options INTERFACE "-fconcepts")
ENDIF()

include_directories(include)

add_subdirectory(examples)
add_subdirectory(tst)
