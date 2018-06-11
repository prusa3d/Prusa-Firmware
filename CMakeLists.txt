cmake_minimum_required(VERSION 3.0)

project(cmake_test)

# Prepare "Catch" library for other executables
set(CATCH_INCLUDE_DIR Catch2)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

# Make test executable
set(TEST_SOURCES 
	Tests/tests.cpp
	Tests/Example_test.cpp
)
add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests Catch)
