add_executable(ParameterMap_gTest ParameterMap_gTest.cpp) 

target_link_libraries(ParameterMap_gTest project_options gtest gtest_main pthread)

add_test(NAME ParameterMap COMMAND ParameterMap_gTest)