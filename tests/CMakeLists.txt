# do not build the firmware by default (tests are the focus if not crosscompiling)
project(cmake_test)

# include catch_discover_tests function from Catch2
include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

# Make test executable
set(TEST_SOURCES
	Example_test.cpp
	PrusaStatistics_test.cpp
    #Tests/Timer_test.cpp
    #Firmware/Timer.cpp
	)

add_executable(tests ${TEST_SOURCES})
target_include_directories(tests PRIVATE tests)
target_link_libraries(tests Catch2::Catch2WithMain)
catch_discover_tests(tests)
