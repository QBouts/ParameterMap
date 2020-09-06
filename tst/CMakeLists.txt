find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(ParameterMap_gTest ParameterMap_gTest.cpp) 

target_link_libraries(ParameterMap_gTest project_options ${GTEST_BOTH_LIBRARIES} pthread)

add_test(NAME ParameterMap COMMAND ParameterMap_gTest)