#[[
# This file is responsible for setting the following variables:
#
# ~~~
# FW_MAJOR (3)
# FW_MINOR (13)
# FW_REVISION (2)
# FW_FLAVOR (RC) RC|BETA|ALPHA|DEV
# FW_FLAVERSION (1)
# FW_COMMIT (7569)
# PROJECT_FW_VERSION (3.13.2)
# FW_COMMIT_DSC ("v3.13.2-RC1-deadbeef")
# FW_COMMIT_HASH (deadbeef)
# FW_COMMIT_DATE (1665051856)
#
# The `PROJECT_FW_VERSION` variable is set as soon as the file is included.
# To set the rest, the function `resolve_version_variables` has to be called.
# ~~~
#]]

function(resolve_version_variables)
  if(FW_COMMIT_DSC)
    return()
  endif()
  if(NOT GIT_FOUND)
    find_package(Git QUIET)
  endif()
  git_head_commit_data(FW_COMMIT_HASH "%h")
  set(ERRORS "GIT-NOTFOUND" "HEAD-FORMAT-NOTFOUND")
  if(FW_COMMIT_HASH IN_LIST ERRORS)
    message(STATUS "FW_COMMIT_HASH IN_LIST ERRORS")
    # git not available, set fallback values reading Firmware/Configuration.h file
    file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/Firmware/Configuration.h CFG_VER_DATA
    REGEX "^#define FW_[A-Z_]+ ([A-Z0-9]+)"
    )
    list(GET CFG_VER_DATA 0 FW_MAJOR)
    list(GET CFG_VER_DATA 1 FW_MINOR)
    list(GET CFG_VER_DATA 2 FW_REVISION)
    list(GET CFG_VER_DATA 3 FW_FLAVOR)
    list(GET CFG_VER_DATA 4 FW_FLAVERSION)
    string(REGEX MATCH "FW_MAJOR ([0-9]+)" FW_MAJOR "${FW_MAJOR}")
    set(FW_MAJOR ${CMAKE_MATCH_1})
    string(REGEX MATCH "FW_MINOR ([0-9]+)" FW_MINOR "${FW_MINOR}")
    set(FW_MINOR ${CMAKE_MATCH_1})
    string(REGEX MATCH "FW_REVISION +([0-9]+)" FW_REVISION "${FW_REVISION}")
    set(FW_REVISION ${CMAKE_MATCH_1})
    string(REGEX MATCH "^#define FW_FLAVOR +([A-Z]+)" FW_FLAVOR "${FW_FLAVOR}")
    set(FW_FLAVOR ${CMAKE_MATCH_1})
    string(REGEX MATCH "^#define FW_FLAVERSION +([0-9]+)" FW_FLAVERSION "${FW_FLAVERSION}")
    set(FW_FLAVERSION ${CMAKE_MATCH_1})
    set(PROJECT_FW_VERSION "${FW_MAJOR}.${FW_MINOR}.${FW_REVISION}")
    set(PROJECT_VERSION_COMMIT_NR ${FW_COMMIT_NR})
    set(FW_DEV_VERSION "${FW_DEV_VERSION}")
    set(FW_COMMIT_HASH "UNKNOWN")
    set(FW_COMMIT_DSC "${PROJECT_FW_VERSION}")
    string(TIMESTAMP FW_COMMIT_DATE "%s")
    set(FW_REPOSITORY "Unknown")
  else()
    message(STATUS "FW_COMMIT_HASH FOUND")
    git_describe_working_tree(FW_COMMIT_DSC)
    #set(FW_COMMIT_DSC "v3.13.2-RC1")
    git_head_commit_data(FW_COMMIT_DATE "%ct")
    git_head_commit_number(FW_COMMIT_NR)
    git_get_repository(FW_REPOSITORY)
  endif()
  
  message(STATUS "1: ${FW_COMMIT_DSC}") 
  string(REPLACE "v" "" FW_COMMIT_DSC ${FW_COMMIT_DSC}) 
  message(STATUS "2: ${FW_COMMIT_DSC}") 
  string(FIND ${FW_COMMIT_DSC} "-" HAS_FALVOR)
  message(STATUS "has falvor: ${HAS_FALVOR}")
  if(${HAS_FALVOR} GREATER_EQUAL 5)
    string(REGEX MATCH "([0-9]+).([0-9]+).([0-9]+)-([A-Z0-9]+)"
           PROJECT_VERSION_LIST "${FW_COMMIT_DSC}")
  else()
    string(REGEX MATCH "([0-9]+).([0-9]+).([0-9])"
           PROJECT_VERSION_LIST "${FW_COMMIT_DSC}")
  endif()
  message(STATUS "3: ${PROJECT_VERSION_LIST}")
  set(FW_MAJOR ${CMAKE_MATCH_1})
  set(FW_MINOR ${CMAKE_MATCH_2})
  set(FW_REVISION ${CMAKE_MATCH_3})
  if(${HAS_FALVOR} GREATER_EQUAL 5)
    set(FW_FLAVOR ${CMAKE_MATCH_4})
    set(FW_FLAVERSION ${CMAKE_MATCH_4})
    string(REGEX REPLACE "[0-9]" "" FW_FLAVOR ${FW_FLAVOR})
    string(REGEX REPLACE "[A-Z]" "" FW_FLAVERSION ${FW_FLAVERSION})
    set(FW_FLAVOR ${FW_FLAVOR})
    set(FW_FLAVERSION ${FW_FLAVERSION})
  endif()

  if("${FW_FLAVOR}" STREQUAL "")
    set(FW_DEV_VERSION "FW_VERSION_GOLD")
  elseif("${FW_FLAVOR}" STREQUAL "RC")
    set(FW_DEV_VERSION "FW_VERSION_RC")
  elseif("${FW_FLAVOR}" STREQUAL "BETA")
    set(FW_DEV_VERSION "FW_VERSION_BETA")
  elseif("${FW_FLAVOR}" STREQUAL "ALPHA")
    set(FW_DEV_VERSION "FW_VERSION_ALPHA")
  else()
    set(FW_DEV_VERSION "FW_VERSION_UNKNOWN")
  endif()

  message(STATUS "3.1: -${FW_MAJOR}-")
  message(STATUS "3.2: -${FW_MINOR}-")
  message(STATUS "3.3: -${FW_REVISION}-")
  message(STATUS "3.4: -${FW_FLAVOR}-")
  message(STATUS "3.5: -${FW_FLAVERSION}-")

  set(FW_MAJOR
      ${FW_MAJOR}
      PARENT_SCOPE
     )
  set(FW_MINOR
      ${FW_MINOR}
      PARENT_SCOPE
     )
  set(FW_REVISION
      ${FW_REVISION}
      PARENT_SCOPE
     )
  set(FW_FLAVOR
      "${FW_FLAVOR}"
      PARENT_SCOPE
     )
  set(FW_FLAVERSION
      ${FW_FLAVERSION}
      PARENT_SCOPE
     )
  set(FW_VERSION
      "${FW_VERSION}"
      PARENT_SCOPE
     )
  set(FW_COMMIT_DSC
      "${FW_COMMIT_DSC}"
      PARENT_SCOPE
     )
  set(FW_COMMIT_HASH
      "${FW_COMMIT_HASH}"
      PARENT_SCOPE
     )
  set(FW_COMMIT_NR
      ${FW_COMMIT_NR}
      PARENT_SCOPE
     )
  set(FW_COMMIT_DATE
      "${FW_COMMIT_DATE}"
      PARENT_SCOPE
     )
  set(FW_DEV_VERSION
      "${FW_DEV_VERSION}"
      PARENT_SCOPE
     )
  # PROJECT_VERSION_TIMESTAMP
  if(NOT PROJECT_VERSION_TIMESTAMP)
    git_head_commit_timestamp(timestamp)
    set(PROJECT_VERSION_TIMESTAMP
        "${timestamp}"
        PARENT_SCOPE
        )
  endif()

  set(FW_REPOSITORY
      "${FW_REPOSITORY}"
      PARENT_SCOPE
      )

endfunction()
