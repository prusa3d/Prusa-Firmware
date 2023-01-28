get_filename_component(PROJECT_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)
get_filename_component(PROJECT_ROOT_DIR "${PROJECT_CMAKE_DIR}" DIRECTORY)

find_package(Python3 COMPONENTS Interpreter)
if(NOT Python3_FOUND)
  message(FATAL_ERROR "Python3 not found.")
endif()

function(get_recommended_gcc_version var)
  execute_process(
    COMMAND "${Python3_EXECUTABLE}" "${PROJECT_ROOT_DIR}/utils/bootstrap.py"
            "--print-dependency-version" "avr-gcc"
    OUTPUT_VARIABLE RECOMMENDED_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE RETVAL
    )

  if(NOT "${RETVAL}" STREQUAL "0")
    message(FATAL_ERROR "Failed to obtain recommended gcc version from utils/bootstrap.py")
  endif()

  set(${var}
      ${RECOMMENDED_VERSION}
      PARENT_SCOPE
      )
endfunction()

function(get_dependency_directory dependency var)
  execute_process(
    COMMAND "${Python3_EXECUTABLE}" "${PROJECT_ROOT_DIR}/utils/bootstrap.py"
            "--print-dependency-directory" "${dependency}"
    OUTPUT_VARIABLE DEPENDENCY_DIRECTORY
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE RETVAL
    )

  if(NOT "${RETVAL}" STREQUAL "0")
    message(FATAL_ERROR "Failed to find directory with ${dependency}")
  endif()

  file(TO_CMAKE_PATH "${DEPENDENCY_DIRECTORY}" DEPENDENCY_DIRECTORY)
  set(${var}
      ${DEPENDENCY_DIRECTORY}
      PARENT_SCOPE
      )
endfunction()

function(objcopy target format suffix)
  add_custom_command(
    TARGET ${target} POST_BUILD
    COMMAND "${CMAKE_OBJCOPY}" -O ${format} -S "$<TARGET_FILE:${target}>"
            "${CMAKE_CURRENT_BINARY_DIR}/${target}${suffix}"
    COMMENT "Generating ${format} from ${target}..."
    BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/${target}${suffix}"
    )
endfunction()

function(report_size target)
  add_custom_command(
    TARGET ${target} POST_BUILD
    COMMAND "${CMAKE_SIZE_UTIL}" -B "$<TARGET_FILE:${target}>"
    USES_TERMINAL
    )
endfunction()
