#[[
# This file is responsible for setting the following variables:
#
# ~~~
# PROJECT_VERSION_MAJOR (3)
# PROJECT_VERSION_MINOR (13)
# PROJECT_VERSION_REV (2)
# PROJECT_VERSION (3.13.2)
# FW_COMMIT_DSC ("v3.13.2-deadbeef")
# FW_COMMIT_HASH (deadbeef)
# FW_COMMIT_DATE (1665051856)
#
# The `PROJECT_VERSION` variable is set as soon as the file is included.
# To set the rest, the function `resolve_version_variables` has to be called.
# ~~~
#]]

# NOTE: these are ordered according to the enum in Util.h. DO NOT ALTER!
# // Definition of a firmware flavor numerical values.
# // To keep it short as possible
# // DEVs/ALPHAs/BETAs limited to max 8 flavor versions
# // RCs limited to 32 flavor versions
# // Final Release always 64 as highest
# enum FirmwareRevisionFlavorType : uint16_t {
#     FIRMWARE_REVISION_RELEASED = 0x0040,
#     FIRMWARE_REVISION_DEV = 0x0000,
#     FIRMWARE_REVISION_ALPHA = 0x008,
#     FIRMWARE_REVISION_BETA = 0x0010,
#     FIRMWARE_REVISION_RC = 0x0020
# };

# Note - the Xes are padding because there is an extended allowance of RC numbers.
SET(DEV_TAGS "DEV" "ALPHA" "BETA" "RC" "X" "X" "X" "X" "RELEASED")
STRING(REPLACE ";" "\|" DEV_TAG_REGEX "${DEV_TAGS}")
function(decode_flavor_code _output _label _ver)
    LIST(FIND DEV_TAGS "${_label}" _code_index)
    MESSAGE(DEBUG "Tweak math: (8*0${_code_index})+${_ver}")
        MATH(EXPR _decoded "(8*0${_code_index})+${_ver}")
    set(${_output}
        "${_decoded}"
        PARENT_SCOPE)
endfunction()

function(decode_tweak_version _str _ver)
    MATH(EXPR _string_id "(${PROJECT_VERSION_TWEAK})/8")
    MATH(EXPR _version "${PROJECT_VERSION_TWEAK}-(8*${_string_id})")
    set(${_ver} "${_version}" PARENT_SCOPE)
    LIST(GET DEV_TAGS ${_string_id} _string_val)
    set(${_str} "${_string_val}" PARENT_SCOPE)
endfunction()


file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/Firmware/Configuration.h CFG_VER_DATA
     REGEX "#define FW_[A-Z]+ (${DEV_TAG_REGEX}|[0-9]+)"
     )
list(GET CFG_VER_DATA 0 PROJECT_VERSION_MAJOR)
list(GET CFG_VER_DATA 1 PROJECT_VERSION_MINOR)
list(GET CFG_VER_DATA 2 PROJECT_VERSION_REV)
list(GET CFG_VER_DATA 3 PROJECT_VERSION_COMMIT)
list(GET CFG_VER_DATA 4 PROJECT_VERSION_FLV)
list(GET CFG_VER_DATA 5 PROJECT_VERSION_FLV_VER)

string(REGEX MATCH "FW_MAJOR ([0-9]+)" PROJECT_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(PROJECT_VERSION_MAJOR "${CMAKE_MATCH_1}")

string(REGEX MATCH "FW_MINOR ([0-9]+)" PROJECT_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(PROJECT_VERSION_MINOR ${CMAKE_MATCH_1})

string(REGEX MATCH "FW_REVISION +([0-9]+)" PROJECT_VERSION_REV "${PROJECT_VERSION_REV}")
set(PROJECT_VERSION_REV ${CMAKE_MATCH_1})

string(REGEX MATCH "FW_COMMITNR +([0-9]+)" PROJECT_VERSION_COMMIT "${PROJECT_VERSION_COMMIT}")
set(PROJECT_VERSION_COMMIT ${CMAKE_MATCH_1})

string(REGEX MATCH "^#define FW_FLAVOR +(${DEV_TAG_REGEX})" PROJECT_VERSION_FLV "${PROJECT_VERSION_FLV}")
IF(PROJECT_VERSION_FLV)
    #message(STATUS "Found a defined FW_FLAVOR")
    set(PROJECT_VERSION_FLV ${CMAKE_MATCH_1})
    string(REGEX MATCH "^#define FW_FLAVERSION +([0-9]+)" PROJECT_VERSION_FLV_VER "${PROJECT_VERSION_FLV_VER}")
    set(PROJECT_VERSION_FLV_VER ${CMAKE_MATCH_1})
    decode_flavor_code(PROJECT_VERSION_TWEAK "${PROJECT_VERSION_FLV}" "${PROJECT_VERSION_FLV_VER}")
ELSE()
    set(PROJECT_VERSION_FLV_VER "")
    decode_flavor_code(PROJECT_VERSION_TWEAK "RELEASED" "0")
ENDIF()

set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_REV}.${PROJECT_VERSION_TWEAK}")

# Define a constant length for the commit hash
set(FW_COMMIT_HASH_LENGTH 9)

# Create fallback value with constant length
string(REPEAT "0" ${FW_COMMIT_HASH_LENGTH} FW_COMMIT_HASH_UNKNOWN)

function(resolve_version_variables)
  if(FW_COMMIT_DSC)
    return()
  endif()
  if(NOT GIT_FOUND)
    find_package(Git QUIET)
  endif()

  # Get the full commit hash
  git_head_commit_data(FW_COMMIT_HASH "%H")

  # Keep only the first 'FW_COMMIT_HASH_LENGTH' characters
  string(SUBSTRING "${FW_COMMIT_HASH}" 0 ${FW_COMMIT_HASH_LENGTH} FW_COMMIT_HASH)

  set(ERRORS "GIT-NOTFOUND" "HEAD-FORMAT-NOTFOUND")
  if(FW_COMMIT_HASH IN_LIST ERRORS)
    # git not available, set fallback values
    set(FW_COMMIT_HASH ${FW_COMMIT_HASH_UNKNOWN})
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
