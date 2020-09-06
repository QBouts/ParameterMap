cmake_minimum_required(VERSION 3.11)
project(ParameterMap)

add_library(project_options INTERFACE)
IF(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang") 
target_compile_options(project_options INTERFACE "-std=c++2a")
set (CMAKE_EXE_LINKER_FLAGS "${$CMAKE_EXE_LINKER_FLAGS} -lstdc++ -lm")
ELSE()
target_compile_options(project_options INTERFACE "-std=c++17")
target_compile_options(project_options INTERFACE "-fconcepts")
ENDIF()

include_directories(include)

add_subdirectory(examples)

enable_testing()
add_subdirectory(tst)