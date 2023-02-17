#
# Functions and utilities for build reproducibility
#

# Set a target to be reproducible
function(set_reproducible_target target)
  # properties for static libraries
  set_target_properties(${target} PROPERTIES STATIC_LIBRARY_OPTIONS "-D")

  # properties on executables
  target_link_options(${target} PRIVATE -fdebug-prefix-map=${CMAKE_SOURCE_DIR}=)
  target_link_options(${target} PRIVATE -fdebug-prefix-map=${CMAKE_BINARY_DIR}=)
  if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "8")
    target_compile_options(${target} PRIVATE -ffile-prefix-map=${CMAKE_SOURCE_DIR}=)
  endif()

  # properties on sources
  get_target_property(sources ${target} SOURCES)
  get_target_property(source_dir ${target} SOURCE_DIR)
  foreach(file IN LISTS sources)
    cmake_path(ABSOLUTE_PATH file BASE_DIRECTORY ${source_dir})
    cmake_path(RELATIVE_PATH file BASE_DIRECTORY ${CMAKE_SOURCE_DIR} OUTPUT_VARIABLE rpath)
    set_property(
      SOURCE ${file}
      DIRECTORY ${source_dir}
      APPEND
      PROPERTY COMPILE_OPTIONS "-frandom-seed=${rpath}"
      )
  endforeach()
endfunction()

# Get the list of targets for all directories
function(get_all_targets _result _dir)
  get_property(
    _subdirs
    DIRECTORY "${_dir}"
    PROPERTY SUBDIRECTORIES
    )
  foreach(_subdir IN LISTS _subdirs)
    get_all_targets(${_result} "${_subdir}")
  endforeach()
  get_directory_property(_sub_targets DIRECTORY "${_dir}" BUILDSYSTEM_TARGETS)
  set(${_result}
      ${${_result}} ${_sub_targets}
      PARENT_SCOPE
      )
endfunction()

# Make every target reproducible
function(set_all_targets_reproducible)
  get_all_targets(targets ${CMAKE_SOURCE_DIR})
  foreach(target IN LISTS targets)
    set_reproducible_target(${target})
  endforeach()
endfunction()

# Set source epoch
function(set_source_epoch epoch)
  set(ENV{SOURCE_DATE_EPOCH} ${epoch})
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8")
    string(TIMESTAMP SOURCE_DATE_EPOCH "%Y-%m-%d")
    add_compile_definitions(SOURCE_DATE_EPOCH="${SOURCE_DATE_EPOCH}")
    string(TIMESTAMP SOURCE_TIME_EPOCH "%H:%M:%S")
    add_compile_definitions(SOURCE_TIME_EPOCH="${SOURCE_TIME_EPOCH}")
  endif()
endfunction()
