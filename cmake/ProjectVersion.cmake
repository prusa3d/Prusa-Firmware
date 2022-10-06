#[[
# This file is responsible for setting the following variables:
#
# ~~~
# PROJECT_VERSION_MAJOR (4)
# PROJECT_VERSION_MINOR (0)
# PROJECT_VERSION_REV (3)
# PROJECT_VERSION (4.0.3)
# FW_COMMIT_DSC ("v4.0.3-deadbeef")
# FW_COMMIT_HASH (deadbeef)
# FW_COMMIT_DATE (1665051856)
#
# The `PROJECT_VERSION` variable is set as soon as the file is included.
# To set the rest, the function `resolve_version_variables` has to be called.
# ~~~
#]]

file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/Firmware/Configuration.h CFG_VER_DATA
     REGEX "#define FW_[A-Z]+ ([0-9]+)"
     )
list(GET CFG_VER_DATA 0 PROJECT_VERSION_MAJOR)
list(GET CFG_VER_DATA 1 PROJECT_VERSION_MINOR)
list(GET CFG_VER_DATA 2 PROJECT_VERSION_REV)

string(REGEX MATCH "FW_MAJOR ([0-9]+)" PROJECT_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(PROJECT_VERSION_MAJOR "${CMAKE_MATCH_1}")

string(REGEX MATCH "FW_MINOR ([0-9]+)" PROJECT_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(PROJECT_VERSION_MINOR ${CMAKE_MATCH_1})

string(REGEX MATCH "FW_REVISION +([0-9]+)" PROJECT_VERSION_REV "${PROJECT_VERSION_REV}")
set(PROJECT_VERSION_REV ${CMAKE_MATCH_1})

set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_REV}")

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
    # git not available, set fallback values
    set(FW_COMMIT_HASH "UNKNOWN")
    set(FW_COMMIT_DSC "v${PROJECT_VERSION}-${FW_COMMIT_HASH}")
    string(TIMESTAMP FW_COMMIT_DATE "%s")
  else()
    git_describe_working_tree(FW_COMMIT_DSC)
    git_head_commit_data(FW_COMMIT_DATE "%ct")
  endif()
  set(FW_COMMIT_DSC
      "${FW_COMMIT_DSC}"
      PARENT_SCOPE
      )
  set(FW_COMMIT_HASH
      "${FW_COMMIT_HASH}"
      PARENT_SCOPE
      )
  set(FW_COMMIT_DATE
      "${FW_COMMIT_DATE}"
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

endfunction()
