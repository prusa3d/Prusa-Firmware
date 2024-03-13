cmake_minimum_required(VERSION 3.19)
include(cmake/Utilities.cmake)
include(cmake/GetGitRevisionDescription.cmake)
include(cmake/ReproducibleBuild.cmake)
include(cmake/ProjectVersion.cmake)
resolve_version_variables()

OPTION(ENFORCE_VERSION_MATCH "Enforce checking that configuration.h matches any parsed git tags" OFF)
OPTION(NO_TAG_IS_FATAL "If tag parsing fails, issue a fatal error" OFF)

set(PROJECT_VERSION_HASH
    "<auto>"
    CACHE
      STRING
      "Version suffix to be appended to the final filename (<ver+PROJECT_VERSION_HASH>). Overrides git hash if set."
    )
if(PROJECT_VERSION_HASH STREQUAL "<auto>")
  set(PROJECT_VERSION_HASH "${FW_COMMIT_HASH}")
endif()
set(PROJECT_VERSION_FULL
    "<auto>"
    CACHE
      STRING
      "Full version string to be shown on the info screen in settings. Overrides git version if set."
    )
if(PROJECT_VERSION_FULL STREQUAL "<auto>")
  set(PROJECT_VERSION_FULL "${FW_COMMIT_DSC}")
endif()
set(PROJECT_REPOSITORY
    "Unknown"
    CACHE STRING "Repository string to be shown on the info screen in settings."
    )

set(CUSTOM_COMPILE_OPTIONS
    ""
    CACHE STRING "Allows adding custom C/C++ flags"
    )

#set(FN_VERSION_SUFFIX "FW${PROJECT_VERSION}+${PROJECT_VERSION_HASH}")
set(FN_VERSION_SUFFIX "FW_${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_REV}")
if(PROJECT_VERSION_FLV AND PROJECT_VERSION_FLV_VER)
  set (FN_VERSION_SUFFIX "${FN_VERSION_SUFFIX}-${PROJECT_VERSION_FLV}${PROJECT_VERSION_FLV_VER}")
endif()
set(FN_VERSION_DEBUG_SUFFIX "${FN_VERSION_SUFFIX}+${PROJECT_VERSION_COMMIT}")

# Inform user about the resolved settings from Configuration.h
message(STATUS "Project version (Configuration.h): ${PROJECT_VERSION}")
#message(STATUS "Project version major............: ${PROJECT_VERSION_MAJOR}") #For debuging
#message(STATUS "Project version minor............: ${PROJECT_VERSION_MINOR}") #For debuging
#message(STATUS "Project version revision.........: ${PROJECT_VERSION_REV}") #For debuging
#message(STATUS "Project version flavor...........: ${PROJECT_VERSION_FLV}") #For debuging
#message(STATUS "Project version fla-revison......: ${PROJECT_VERSION_FLV_VER}") #For debuging
#message(STATUS "Project version commit number....: ${PROJECT_VERSION_COMMIT}") #For debuging
message(STATUS "Filename suffix..................: ${FN_VERSION_SUFFIX}")
message(STATUS "Filename debug suffix ...........: ${FN_VERSION_DEBUG_SUFFIX}")
# testing
# SET(FW_COMMIT_DSC "v3.13.0-1234")

if(NOT "${PROJECT_VERSION_HASH}" STREQUAL "UNKNOWN" AND NOT "${FW_COMMIT_DSC}" MATCHES ".+NOTFOUND.+") # else -> no commit hash is known... likely no git.
string(REGEX MATCH "[v|t]([0-9]+)\.([0-9]+)\.([0-9]+)-?(${DEV_TAG_REGEX})?([0-9]+)?-([0-9]+)" TAG_VERSION "${FW_COMMIT_DSC}")

if (CMAKE_MATCH_4) # Do we have a build type?
    decode_flavor_code(PROJECT_VER_TAG_FLV "${CMAKE_MATCH_4}" "${CMAKE_MATCH_5}")
else()
    # No dev status found, it must be a final tag.
    decode_flavor_code(PROJECT_VER_TAG_FLV "RELEASED" "0")
endif()

if(ENFORCE_VERSION_MATCH)
    if(NOT ${CMAKE_MATCH_1} STREQUAL ${PROJECT_VERSION_MAJOR})
        message(FATAL_ERROR "Major version of current tag disagrees with Configuration.h ${CMAKE_MATCH_1}!=${PROJECT_VERSION_MAJOR}")
    endif()

    if(NOT ${CMAKE_MATCH_2} STREQUAL ${PROJECT_VERSION_MINOR})
        message(FATAL_ERROR "Minor version of current tag disagrees with Configuration.h ${CMAKE_MATCH_2}!=${PROJECT_VERSION_MINOR}")
    endif()

    if(NOT ${CMAKE_MATCH_3} STREQUAL ${PROJECT_VERSION_REV})
        message(FATAL_ERROR "Rev version of current tag disagrees with Configuration.h ${CMAKE_MATCH_3}!=${PROJECT_VERSION_REV}")
    endif()

    if(NOT ${PROJECT_VER_TAG_FLV} STREQUAL ${PROJECT_VERSION_TWEAK})
        message(FATAL_ERROR "Dev status of current tag disagrees with Configuration.h ${PROJECT_VER_TAG_FLV}!=${PROJECT_VERSION_TWEAK}")
    endif()
    # Note - we don't check the commit counter, that'd be too much of a headache. Maybe it
    # should be an error only on a tagged build?
    MESSAGE(STATUS "Configuration.h and tag match: OK (${PROJECT_VERSION}/${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}.${PROJECT_VER_TAG_FLV})")
else()
    MESSAGE(STATUS "Configuration.h and tag (not enforced): (${PROJECT_VERSION}/${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}.${PROJECT_VER_TAG_FLV})")
endif()

MESSAGE(STATUS "Commit Nr: Configuration.h: ${PROJECT_VERSION_COMMIT} Tag: ${CMAKE_MATCH_6}")
MESSAGE(STATUS "These tag values will override Configuration.h")
SET(PROJECT_VERSION ${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}.${PROJECT_VER_TAG_FLV})
SET(PROJECT_VERSION_COMMIT ${CMAKE_MATCH_6})

git_get_repository(PROJECT_REPOSITORY)
else(GIT_FOUND)
if (NO_TAG_IS_FATAL)
    MESSAGE(FATAL_ERROR "Git was not found or an error occurred parsing the tag. This is a fatal error according to the settings.")
else()
    MESSAGE(STATUS "Git was not found or an error occurred parsing the tag. Falling back to Configuration.h values (${PROJECT_VERSION}).")
endif()
set(FW_COMMIT_HASH ${FW_COMMIT_HASH_UNKNOWN}) # Clear it, the code expects a binary...
set(PROJECT_VERSION_TIMESTAMP "0")
endif()

if(CMAKE_MATCH_1 AND CMAKE_MATCH_2)
  set(FN_VERSION_SUFFIX "FW_${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}")
endif()
if(CMAKE_MATCH_4 AND CMAKE_MATCH_5)
  set (FN_VERSION_SUFFIX "${FN_VERSION_SUFFIX}-${CMAKE_MATCH_4}${CMAKE_MATCH_5}")
endif()
if(CMAKE_MATCH_6 AND PROJECT_VERSION_HASH)
  set(FN_VERSION_DEBUG_SUFFIX "${FN_VERSION_SUFFIX}+${CMAKE_MATCH_6}_${PROJECT_VERSION_HASH}")
endif()
# Inform user about the resolved settings from github
message(STATUS "Project version git..............: ${PROJECT_VERSION}")
message(STATUS "Project version git hash.........: ${PROJECT_VERSION_HASH}")
message(STATUS "Project version git description..: ${PROJECT_VERSION_FULL}")
#message(STATUS "Project version git major........: ${CMAKE_MATCH_1}") #For debuging
#message(STATUS "Project version git minor........: ${CMAKE_MATCH_2}") #For debuging
#message(STATUS "Project version git revision.....: ${CMAKE_MATCH_3}") #For debuging
#message(STATUS "Project version git flavor.......: ${CMAKE_MATCH_4}") #For debuging
#message(STATUS "Project version git fla-revison..: ${CMAKE_MATCH_5}") #For debuging
#message(STATUS "Project version git commit number: ${CMAKE_MATCH_6}") #For debuging
message(STATUS "Filename suffix .................: ${FN_VERSION_SUFFIX}")
message(STATUS "Filename debug suffix ...........: ${FN_VERSION_DEBUG_SUFFIX}")

# Language configuration
set(MAIN_LANGUAGES
    cs de es fr it pl
    CACHE STRING "The list of 'main' languages to be included, in the correct order"
    )
set(COMMUNITY_LANGUAGES
    nl
    ro
    hu
    hr
    sk
    sv
    no
    CACHE STRING "The list of community languages to be included, in the correct order"
    )
set(SELECTED_LANGUAGES ${MAIN_LANGUAGES} ${COMMUNITY_LANGUAGES})

get_dependency_directory(prusa3dboards PRUSA_BOARDS_DIR)
project(Prusa-Firmware VERSION ${PROJECT_VERSION})
add_subdirectory(lib)

# Get LANG_MAX_SIZE from sources
file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/Firmware/config.h MAX_SIZE_LINE
     REGEX "^#define \+LANG_SIZE_RESERVED \+"
     )
string(REGEX MATCH "0x[0-9]+" MAX_SIZE_HEX "${MAX_SIZE_LINE}")
math(EXPR LANG_MAX_SIZE "${MAX_SIZE_HEX}" OUTPUT_FORMAT DECIMAL)
message("Language maximum size (from config.h): ${LANG_MAX_SIZE} bytes")

# Ditto, this in xflash_layout.h but needs invocation of the preprocessor... :-/
set(LANG_BIN_MAX 249856)

# Check GCC Version
get_recommended_gcc_version(RECOMMENDED_TOOLCHAIN_VERSION)
if(CMAKE_CROSSCOMPILING AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_EQUAL
                            ${RECOMMENDED_TOOLCHAIN_VERSION}
   )
  message(WARNING "Recommended AVR toolchain is ${RECOMMENDED_TOOLCHAIN_VERSION}"
                  ", but you have ${CMAKE_CXX_COMPILER_VERSION}"
          )

elseif(NOT CMAKE_CROSSCOMPILING AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  message(
    WARNING
      "Recommended compiler for host tools and unittests is GCC, you have ${CMAKE_CXX_COMPILER_ID}."
    )
endif()

# append custom C/C++ flags
if(CUSTOM_COMPILE_OPTIONS)
  string(REPLACE " " ";" CUSTOM_COMPILE_OPTIONS "${CUSTOM_COMPILE_OPTIONS}")
  add_compile_options(${CUSTOM_COMPILE_OPTIONS})
endif()

#
# Global Compiler & Linker Configuration
#

# enable warnings
add_compile_options(-Wall -Wextra -Wno-expansion-to-defined -Wsign-compare)

# default standards for all targets
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# support _DEBUG macro (some code uses to recognize debug builds)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_definitions(_DEBUG)
endif()

#
# Firmware - get file lists.
#
set(FW_SOURCES
    adc.cpp
    backlight.cpp
    BlinkM.cpp
    bootapp.c
    cardreader.cpp
    cmdqueue.cpp
    Configuration.cpp
    ConfigurationStore.cpp
    Dcodes.cpp
    eeprom.cpp
    fancheck.cpp
    Filament_sensor.cpp
    first_lay_cal.cpp
    heatbed_pwm.cpp
    host.cpp
    la10compat.cpp
    language.c
    lcd.cpp
    Marlin_main.cpp
    MarlinSerial.cpp
    meatpack.cpp
    menu.cpp
    mesh_bed_calibration.cpp
    mesh_bed_leveling.cpp
    messages.cpp
    mmu2.cpp
    mmu2_crc.cpp
    mmu2_error_converter.cpp
    mmu2_fsensor.cpp
    mmu2_log.cpp
    mmu2_marlin1.cpp
    mmu2_power.cpp
    mmu2_progress_converter.cpp
    mmu2_protocol.cpp
    mmu2_protocol_logic.cpp
    mmu2_reporting.cpp
    mmu2_serial.cpp
    motion_control.cpp
    optiboot_xflash.cpp
    pat9125.cpp
    planner.cpp
    power_panic.cpp
    printer_state.cpp
    Prusa_farm.cpp
    qr_solve.cpp
    rbuf.c
    Sd2Card.cpp
    SdBaseFile.cpp
    SdFatUtil.cpp
    SdFile.cpp
    SdVolume.cpp
    Servo.cpp
    sm4.c
    sound.cpp
    speed_lookuptable.cpp
    spi.c
    SpoolJoin.cpp
    stepper.cpp
    stopwatch.cpp
    strtod.c
    swi2c.c
    Tcodes.cpp
    temperature.cpp
    timer02.c
    Timer.cpp
    tmc2130.cpp
    tone04.c
    twi.cpp
    uart2.c
    ultralcd.cpp
    util.cpp
    vector_3.cpp
    xflash.c
    xflash_dump.cpp
    xyzcal.cpp
    )
list(TRANSFORM FW_SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Firmware/)

set(AVR_SOURCES
    wiring_digital.c
    WInterrupts.c
    wiring_pulse.c
    hooks.c
    wiring.c
    wiring_analog.c
    wiring_shift.c
    CDC.cpp
    PluggableUSB.cpp
    HardwareSerial.cpp
    HardwareSerial0.cpp
    HardwareSerial1.cpp
    HardwareSerial3.cpp
    IPAddress.cpp
    HardwareSerial2.cpp
    Print.cpp
    Stream.cpp
    Tone.cpp
    USBCore.cpp
    WMath.cpp
    WString.cpp
    abi.cpp
    main.cpp
    )
list(TRANSFORM AVR_SOURCES PREPEND ${PRUSA_BOARDS_DIR}/cores/prusa_einsy_rambo/)

#
# Target configuration
#
if(CMAKE_CROSSCOMPILING)
  set_source_epoch(${PROJECT_VERSION_TIMESTAMP})

  # default optimization flags
  set(CMAKE_CXX_FLAGS_DEBUG "-Og -g")
  set(CMAKE_CXX_FLAGS_RELEASE "-Os -g -DNDEBUG")
  set(CMAKE_C_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
  set(CMAKE_C_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})

  # mcu and target-related settings
  add_compile_options(
    -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10819 -DARDUINO_AVR_PRUSA_EINSY_RAMBO
    -DARDUINO_ARCH_AVR
    )
  add_link_options(-mmcu=atmega2560 -Wl,-u,vfprintf -lprintf_flt -lm)

  # disable some C++ language features
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)

  # disable exceptions
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-unwind-tables>)

  # split and gc sections
  add_compile_options(-ffunction-sections -fdata-sections)
  add_link_options(-ffunction-sections -fdata-sections -Wl,--gc-sections)

  # LTO (with custom options)
  add_compile_options(-flto -fno-fat-lto-objects)
  add_link_options(-flto)

  # Create this target before we apply the GC options
  add_library(avr_core STATIC ${AVR_SOURCES})
  set_reproducible_target(avr_core)
  target_include_directories(
    avr_core PRIVATE ${PRUSA_BOARDS_DIR}/cores/prusa_einsy_rambo/
                     ${PRUSA_BOARDS_DIR}/variants/prusa_einsy_rambo/
    )
endif()

# Meta targets to build absolutely everything
add_custom_target(ALL_FIRMWARE)
add_custom_target(ALL_ENGLISH)
add_custom_target(ALL_MULTILANG)
add_dependencies(ALL_FIRMWARE ALL_ENGLISH ALL_MULTILANG)
set_target_properties(ALL_MULTILANG PROPERTIES EXCLUDE_FROM_ALL FALSE)

function(add_base_binary variant_name)
  add_executable(${variant_name} ${FW_SOURCES} ${FW_HEADERS} ${VARIANT_CFG_DST})
  set_target_properties(${variant_name} PROPERTIES EXCLUDE_FROM_ALL TRUE)
  set_reproducible_target(${variant_name})

  target_include_directories(
    ${variant_name}
    PRIVATE ${PRUSA_BOARDS_DIR}/cores/prusa_einsy_rambo/
            ${PRUSA_BOARDS_DIR}/variants/prusa_einsy_rambo/ ${CMAKE_SOURCE_DIR}/Firmware
    )

  target_link_libraries(${variant_name} avr_core)

  # configure linker script
  set(LINKER_SCRIPT ${PRUSA_BOARDS_DIR}/ldscripts/avr6.xn)
  target_link_options(${variant_name} PUBLIC -Wl,-T,${LINKER_SCRIPT})

  # limit the text section to 248K (256K - 8k reserved for the bootloader)
  target_link_options(${variant_name} PUBLIC -Wl,--defsym=__TEXT_REGION_LENGTH__=248K)

  # produce ASM listing. Note we also specify the .map as a byproduct so it gets cleaned because
  # link_options doesn't have a "generated outputs" feature.
  add_custom_command(
    TARGET ${variant_name}
    POST_BUILD
    COMMAND ${CMAKE_OBJDUMP} --prefix ${CMAKE_SOURCE_DIR} -CSd ${variant_name} > ${variant_name}.asm
    BYPRODUCTS ${variant_name}.asm ${variant_name}.map
    )

  # inform about the firmware's size in terminal
  add_custom_command(
    TARGET ${variant_name}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo_append "${variant_name} "
    COMMAND ${CMAKE_SIZE_UTIL} -C --mcu=atmega2560 ${variant_name}
    )
  report_size(${variant_name})

  # generate linker map file
  target_link_options(
    ${variant_name} PUBLIC -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${variant_name}.map
    )
  decode_tweak_version(PROJECT_VERSION_FLV PROJECT_VERSION_FLV_VER)
  target_compile_definitions(
    ${variant_name}
    PRIVATE CMAKE_CONTROL FW_REPOSITORY="${PROJECT_REPOSITORY}"
            FW_COMMIT_HASH="${FW_COMMIT_HASH}"
            FW_COMMIT_HASH_LENGTH=${FW_COMMIT_HASH_LENGTH}
            FW_MAJOR=${PROJECT_VERSION_MAJOR}
            FW_MINOR=${PROJECT_VERSION_MINOR}
            FW_REVISION=${PROJECT_VERSION_REV}
            FW_COMMITNR=${PROJECT_VERSION_COMMIT}
    )
    if(NOT PROJECT_VERSION_FLV STREQUAL "RELEASED")
        target_compile_definitions(
        ${variant_name}
        PRIVATE
        FW_FLAVERSION=${PROJECT_VERSION_FLV_VER}
        FW_FLAVOR=${PROJECT_VERSION_FLV}
        )
    endif()
endfunction()

function(fw_add_variant variant_name)
  set(variant_header "variants/${variant_name}.h")
  string(REPLACE "1_75mm_" "" variant_name "${variant_name}")
  string(REPLACE "-E3Dv6full" "" variant_name "${variant_name}")

  # Single-language build
  set(FW_EN "${variant_name}_ENGLISH")
  #MK3S_MK3S+_FW_3.13.2-RC1_ENGLISH.hex
  set(hex_variant_name "${variant_name}")
  if(hex_variant_name STRGREATER_EQUAL "MK3S")
    string(REPLACE "MK3S" "MK3S_MK3S+" hex_variant_name ${hex_variant_name})
  endif()
  set(FW_HEX "${CMAKE_BINARY_DIR}/${hex_variant_name}_${FN_VERSION_SUFFIX}_ENGLISH.hex")
  #message(STATUS "Hex filename: ${FW_HEX}")

  add_base_binary(${FW_EN})
  target_compile_definitions(${FW_EN} PUBLIC LANG_MODE=0 FW_VARIANT="${variant_header}")
  add_custom_command(
    TARGET ${FW_EN}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${FW_EN} ${FW_EN}.hex
    COMMAND ${CMAKE_COMMAND} -E create_hardlink ${FW_EN}.hex ${FW_HEX}
    BYPRODUCTS ${FW_EN}.hex ${FW_HEX}
    COMMENT "Generating ${FW_EN}.hex"
    )
  add_dependencies(ALL_ENGLISH ${FW_EN})

  # Multi-language build/s
  set(FW_LANG_BASE "${variant_name}_lang_base")
  set(FW_LANG_PATCH "${variant_name}_lang_patch")
  add_base_binary(${FW_LANG_BASE})
  target_compile_definitions(${FW_LANG_BASE} PUBLIC LANG_MODE=1 FW_VARIANT="${variant_header}")

  # Construct language map
  set(LANG_TMP_DIR lang)
  set(LANG_MAP ${LANG_TMP_DIR}/${variant_name}_lang.map)

  add_custom_command(
    OUTPUT ${LANG_MAP}
    COMMAND ${CMAKE_OBJCOPY} -O binary ${FW_LANG_BASE} ${FW_LANG_PATCH}.bin
    COMMAND "${Python3_EXECUTABLE}" ${CMAKE_SOURCE_DIR}/lang/lang-map.py ${FW_LANG_BASE} ${FW_LANG_PATCH}.bin > ${LANG_MAP}
    COMMAND ${CMAKE_OBJCOPY} -I binary -O ihex ${FW_LANG_PATCH}.bin ${FW_LANG_PATCH}.hex
    DEPENDS ${FW_LANG_BASE}
    BYPRODUCTS ${FW_LANG_PATCH}.bin ${FW_LANG_PATCH}.hex
    COMMENT "Generating ${variant_name} language map"
    )

  # Base targets for language checks
  add_custom_target(check_lang_${variant_name})
  add_dependencies(check_lang check_lang_${variant_name})

  # Build language catalogs
  set(LANG_BINS "")
  foreach(LANG IN LISTS SELECTED_LANGUAGES)
    set(LANG_BIN ${LANG_TMP_DIR}/${variant_name}_${LANG}.bin)
    set(PO_FILE "${CMAKE_SOURCE_DIR}/lang/po/Firmware_${LANG}.po")

    # Full language checks
    add_custom_target(
      check_lang_${variant_name}_${LANG}
      COMMENT "Checking ${variant_name} language ${LANG}"
      COMMAND "${Python3_EXECUTABLE}" ${CMAKE_SOURCE_DIR}/lang/lang-check.py --map ${LANG_MAP} ${PO_FILE}
      DEPENDS ${LANG_MAP} ${PO_FILE}
      USES_TERMINAL
      )
    add_dependencies(check_lang_${variant_name} check_lang_${variant_name}_${LANG})
    add_dependencies(check_lang_${LANG} check_lang_${variant_name}_${LANG})

    add_custom_command(
      OUTPUT ${LANG_BIN}
      # Check po file for errors _only_
      COMMAND "${Python3_EXECUTABLE}" ${CMAKE_SOURCE_DIR}/lang/lang-check.py --errors-only --map ${LANG_MAP} ${PO_FILE}
      # Build the catalog
      COMMAND "${Python3_EXECUTABLE}" ${CMAKE_SOURCE_DIR}/lang/lang-build.py ${LANG_MAP} ${PO_FILE} ${LANG_BIN}
      # Check bin size
      COMMAND ${CMAKE_COMMAND} -DLANG_MAX_SIZE=${LANG_MAX_SIZE} -DLANG_FILE=${LANG_BIN} -P
              ${PROJECT_CMAKE_DIR}/Check_lang_size.cmake
      DEPENDS ${LANG_MAP} ${PO_FILE}
      COMMENT "Generating ${variant_name}_${LANG}.bin"
      )
    list(APPEND LANG_BINS ${LANG_BIN})
  endforeach()

  string(FIND ${variant_name} "MK3" HAS_XFLASH)
  if(${HAS_XFLASH} GREATER_EQUAL 0)
    # X-Flash based build (catalogs appended to patched binary)
    set(FW_LANG_FINAL "${variant_name}_MULTILANG")
    set(hex_variant_name "${variant_name}")
    #MK3S_MK3S+_FW_3.13.2-RC1+7651_deadbeef_MULTILANG.hex
    if(hex_variant_name STRGREATER_EQUAL "MK3S")
      string(REPLACE "MK3S" "MK3S_MK3S+" hex_variant_name ${hex_variant_name})
    endif()
    set(LANG_HEX ${CMAKE_BINARY_DIR}/${hex_variant_name}_${FN_VERSION_SUFFIX}_MULTILANG.hex)
    set(LANG_DEBUG_HEX ${CMAKE_BINARY_DIR}/${hex_variant_name}_${FN_VERSION_DEBUG_SUFFIX}_MULTILANG.hex)
    #message(STATUS "Hex filename .....: ${LANG_HEX}")
    #message(STATUS "Hex debug filename: ${LANG_DEBUG_HEX}")
    set(LANG_CATBIN ${LANG_TMP_DIR}/${variant_name}_cat.bin)
    set(LANG_CATHEX ${LANG_TMP_DIR}/${variant_name}_cat.hex)

    add_custom_command(
      OUTPUT ${LANG_CATBIN}
      COMMAND ${CMAKE_COMMAND} -E cat ${LANG_BINS} > ${LANG_CATBIN}
      DEPENDS ${LANG_BINS}
      COMMENT "Merging language catalogs"
      )
    #[[
    #add_custom_command(OUTPUT ${LANG_FINAL_BIN}
    #  COMMAND ${CMAKE_COMMAND} -DLANG_MAX_SIZE=${LANG_BIN_MAX} -DLANG_FILE=${LANG_FINAL_BIN}
    #                           -P ${PROJECT_CMAKE_DIR}/Check_final_lang_bin_size.cmake
    #  APPEND)
    #]]
    add_custom_command(
      OUTPUT ${LANG_CATHEX}
      COMMAND ${CMAKE_OBJCOPY} -I binary -O ihex ${LANG_CATBIN} ${LANG_CATHEX}
      DEPENDS ${LANG_CATBIN}
      COMMENT "Generating Hex for language data"
      )

    add_custom_command(
      OUTPUT ${FW_LANG_FINAL}.hex
      COMMAND ${CMAKE_COMMAND} -E cat ${FW_LANG_PATCH}.hex ${LANG_CATHEX} > ${FW_LANG_FINAL}.hex
      COMMAND ${CMAKE_COMMAND} -E create_hardlink ${FW_LANG_FINAL}.hex ${LANG_HEX}
      BYPRODUCTS ${LANG_HEX}
      COMMAND ${CMAKE_COMMAND} -E create_hardlink ${FW_LANG_FINAL}.hex ${LANG_DEBUG_HEX}
      BYPRODUCTS ${LANG_DEBUG_HEX}
      COMMAND ${CMAKE_COMMAND} -E create_hardlink ${FW_LANG_FINAL}.hex ${CMAKE_BINARY_DIR}/release/${hex_variant_name}_${FN_VERSION_SUFFIX}_MULTILANG.hex
      BYPRODUCTS ${CMAKE_BINARY_DIR}/release/${hex_variant_name}_${FN_VERSION_SUFFIX}_MULTILANG.hex
      DEPENDS ${FW_LANG_PATCH}.hex ${LANG_CATHEX}
      COMMENT "Generating final ${FW_LANG_FINAL}.hex"
      )

    add_custom_target(${FW_LANG_FINAL} DEPENDS ${FW_LANG_FINAL}.hex)
    add_dependencies(ALL_MULTILANG ${FW_LANG_FINAL})
  else()
    set(ALL_VARIANT_HEXES "")
    # Non-xflash, e.g. MK2.5
    foreach(LANG IN LISTS SELECTED_LANGUAGES)
      set(FW_LANG_FINAL ${variant_name}_en-${LANG})
      set(LANG_HEX ${CMAKE_BINARY_DIR}/${variant_name}_${FN_VERSION_SUFFIX}_en-${LANG}.hex)
      set(LANG_DEBUG_HEX ${CMAKE_BINARY_DIR}/${variant_name}_${FN_VERSION_DEBUG_SUFFIX}_en-${LANG}.hex)
      set(LANG_BIN ${LANG_TMP_DIR}/${variant_name}_${LANG}.bin)

      # Patched binary with pre-baked secondary language
      add_custom_command(
        OUTPUT ${FW_LANG_FINAL}.bin
        COMMAND ${CMAKE_OBJCOPY} -O binary ${FW_LANG_BASE} ${FW_LANG_FINAL}.bin
        COMMAND "${Python3_EXECUTABLE}" ${CMAKE_SOURCE_DIR}/lang/lang-patchsec.py ${FW_LANG_BASE} ${LANG_BIN}
                ${FW_LANG_FINAL}.bin
        DEPENDS ${FW_LANG_BASE} ${LANG_BIN}
        COMMENT "Generating ${FW_LANG_FINAL}.bin"
        )

      # Final hex files
      add_custom_command(
        OUTPUT ${FW_LANG_FINAL}.hex
        COMMAND ${CMAKE_OBJCOPY} -I binary -O ihex ${FW_LANG_FINAL}.bin ${FW_LANG_FINAL}.hex
        COMMAND ${CMAKE_COMMAND} -E create_hardlink ${FW_LANG_FINAL}.hex ${LANG_HEX}
        BYPRODUCTS ${LANG_HEX}
        COMMAND ${CMAKE_COMMAND} -E create_hardlink ${FW_LANG_FINAL}.hex ${LANG_DEBUG_HEX}
        BYPRODUCTS ${LANG_DEBUG_HEX}
        DEPENDS ${FW_LANG_FINAL}.bin
        COMMENT "Creating ${FW_LANG_FINAL}.hex"
        )

      add_custom_target(${FW_LANG_FINAL} DEPENDS ${FW_LANG_FINAL}.hex)
      list(APPEND ALL_VARIANT_HEXES ${FW_LANG_FINAL})
    endforeach()
    add_custom_target("${variant_name}-All-Languages" DEPENDS ${ALL_VARIANT_HEXES})
    add_custom_command(TARGET "${variant_name}-All-Languages"
      POST_BUILD
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMAND ${CMAKE_COMMAND} -E make_directory "release"
      COMMAND ${CMAKE_COMMAND} -E tar "cfv" "release/${variant_name}_${FN_VERSION_SUFFIX}.zip" --format=zip ${variant_name}_${FN_VERSION_SUFFIX}_en*.hex
      BYPRODUCTS "${CMAKE_BINARY_DIR}/release/${variant_name}_${FN_VERSION_SUFFIX}.zip"
      )
    add_dependencies(ALL_MULTILANG "${variant_name}-All-Languages")
  endif()
endfunction()

if(CMAKE_CROSSCOMPILING)

  # Main target for language checks
  add_custom_target(check_lang)
  foreach(LANG IN LISTS SELECTED_LANGUAGES)
    add_custom_target(check_lang_${LANG})
    add_dependencies(check_lang check_lang_${LANG})
  endforeach()

  # build a list of all supported variants
  file(
    GLOB ALL_VARIANTS
    RELATIVE ${PROJECT_SOURCE_DIR}/Firmware/variants
    ${PROJECT_SOURCE_DIR}/Firmware/variants/*.h
    )
  list(TRANSFORM ALL_VARIANTS REPLACE "\.h$" "")
  set(FW_VARIANTS
      ${ALL_VARIANTS}
      CACHE STRING "Firmware variants to be built"
      )

  foreach(THIS_VAR IN LISTS FW_VARIANTS)
    if(NOT ${THIS_VAR} IN_LIST ALL_VARIANTS)
      message(FATAL_ERROR "Variant ${THIS_VAR} does not exist")
    endif()

    message("Variant added: ${THIS_VAR}")

    # Generate a file in a subfolder so that we can organize things a little more neatly in VS code
    set(DIR_NAME ${THIS_VAR})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build_gen/${DIR_NAME})
    file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/build_gen/${DIR_NAME}/CMakeLists.txt
         "project(${DIR_NAME} VERSION ${PROJECT_VERSION})\nfw_add_variant(${THIS_VAR})"
         )
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/build_gen/${DIR_NAME})
  endforeach(THIS_VAR IN LISTS FW_VARIANTS)
endif()

SET(REVO_FW_ZIP_NAME "E3D_REVO_FW_MK3_MK3S_MK3S+_${FN_VERSION_SUFFIX}.zip")

add_custom_command(TARGET ALL_MULTILANG
  POST_BUILD
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/release
  BYPRODUCTS ${CMAKE_BINARY_DIR}/release/${REVO_FW_ZIP_NAME}
  COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${REVO_FW_ZIP_NAME}" --format=zip *E3DREVO*.hex
  COMMAND ${CMAKE_COMMAND} -E rm *E3DREVO*.hex
  )

#
# Tests
#
if(NOT CMAKE_CROSSCOMPILING)
  enable_testing()
  add_subdirectory(tests)
endif()
