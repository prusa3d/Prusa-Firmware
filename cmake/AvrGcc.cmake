get_filename_component(PROJECT_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)
include("${PROJECT_CMAKE_DIR}/Utilities.cmake")
get_dependency_directory("avr-gcc" AVR_TOOLCHAIN_DIR)
include("${PROJECT_CMAKE_DIR}/AnyAvrGcc.cmake")
