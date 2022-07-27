cmake_minimum_required(VERSION 3.15)
include(cmake/Utilities.cmake)

set (CMAKE_CXX_STANDARD 11)
project(Prusa-Firmware)

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

# include symbols
add_compile_options(-g)

# optimizations
if(CMAKE_CROSSCOMPILING)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-Og)
  else()
    add_compile_options(-Os)
  endif()

  # mcu related settings
  set(MCU_FLAGS -mmcu=atmega2560 -DF_CPU=16000000L)
  add_compile_options(${MCU_FLAGS})
  add_link_options(${MCU_FLAGS})

  # split and gc sections
  add_compile_options(-ffunction-sections -fdata-sections)
  add_link_options(-Wl,--gc-sections)

  # disable exceptions and related metadata
  add_compile_options(-fno-exceptions -fno-unwind-tables)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
  add_link_options(-Wl,--defsym,__exidx_start=0,--defsym,__exidx_end=0)
else()
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0)
  else()
    add_compile_options(-O2)
  endif()
endif()

# enable all warnings (well, not all, but some)
add_compile_options(-Wall -Wsign-compare)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-std=c++14>)

# support _DEBUG macro (some code uses to recognize debug builds)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_definitions(_DEBUG)
endif()

#
# Firmware - get file lists.
#
file(GLOB FW_SOURCES RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/Firmware/*.c*)
file(GLOB FW_HEADERS RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/Firmware/*.h*)
file(GLOB AVR_SOURCES RELATIVE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/.dependencies/1.8.19-1.0.5-1-linux-64/portable/packages/PrusaResearch/hardware/avr/1.0.5-1/cores/prusa_einsy_rambo/*.c*)

  # Setup language resources:
file(GLOB LANG_VARIANTS RELATIVE ${PROJECT_SOURCE_DIR}/lang/po ${PROJECT_SOURCE_DIR}/lang/po/Firmware_??.po)
string(REPLACE "Firmware_" "" LANG_VARIANTS "${LANG_VARIANTS}")
string(REPLACE ".po" "" LANG_VARIANTS "${LANG_VARIANTS}")
message("Languages found: ${LANG_VARIANTS}")

add_library(avr_core STATIC ${AVR_SOURCES})
target_include_directories(avr_core PRIVATE
  ${PROJECT_SOURCE_DIR}/.dependencies/1.8.19-1.0.5-1-linux-64/portable/packages/PrusaResearch/hardware/avr/1.0.5-1/cores/prusa_einsy_rambo/
  ${PROJECT_SOURCE_DIR}/.dependencies/1.8.19-1.0.5-1-linux-64/portable/packages/PrusaResearch/hardware/avr/1.0.5-1/variants/prusa_einsy_rambo/
)
target_compile_options(avr_core PUBLIC -mmcu=atmega2560)

function(fw_add_variant variant_name)

  add_executable(${variant_name} ${FW_SOURCES} ${FW_HEADERS})

  set_target_properties(${variant_name} PROPERTIES CXX_STANDARD 14)


#   # configure linker script
   set(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/.dependencies/1.8.19-1.0.5-1-linux-64/portable/packages/PrusaResearch/hardware/avr/1.0.5-1/ldscripts/avr6.xn)
   target_link_options(${variant_name} PUBLIC -Wl,-T,${LINKER_SCRIPT})

  # limit the text section to 248K (256K - 8k reserved for the bootloader)
  target_link_options(${variant_name} PUBLIC -Wl,--defsym=__TEXT_REGION_LENGTH__=248K)

  # generate firmware.bin file
  objcopy(${variant_name} "ihex" ".hex")

  # produce ASM listing. Note we also specify the .map as a byproduct so it gets cleaned
  # because link_options doesn't have a "generated outputs" feature.
  add_custom_command(
    TARGET ${variant_name} POST_BUILD COMMAND ${CMAKE_OBJDUMP} -CSd ${variant_name} > ${variant_name}.asm
    BYPRODUCTS ${variant_name}.asm ${variant_name}.map
    )

  # inform about the firmware's size in terminal
  add_custom_command(
    TARGET ${variant_name} POST_BUILD COMMAND ${CMAKE_SIZE_UTIL} -C --mcu=atmega2560 ${variant_name}
    )
  report_size(${variant_name})

  # generate linker map file
  target_link_options(${variant_name} PUBLIC -Wl,-Map=${variant_name}.map)


  target_include_directories(${variant_name} PRIVATE Firmware
    ${PROJECT_SOURCE_DIR}/.dependencies/1.8.19-1.0.5-1-linux-64/portable/packages/PrusaResearch/hardware/avr/1.0.5-1/cores/prusa_einsy_rambo/
    ${PROJECT_SOURCE_DIR}/.dependencies/1.8.19-1.0.5-1-linux-64/portable/packages/PrusaResearch/hardware/avr/1.0.5-1/variants/prusa_einsy_rambo/
    ${PROJECT_SOURCE_DIR}/cmake/helpers/ # Add our magic config helper :)
    )

  target_compile_options(${variant_name} PRIVATE) # turn this on for lolz -Wdouble-promotion)
  string(REPLACE "-" "_" DEFINE_NAME "${variant_name}")
  target_compile_definitions(${variant_name} PRIVATE H${DEFINE_NAME} ARDUINO=10600 __AVR_ATmega2560__)
  target_link_libraries(${variant_name} avr_core)

  #Construct language map
  set(LANG_MAP ${CMAKE_CURRENT_BINARY_DIR}/lang/${variant_name}_lang.map)
  set(LANG_FWBIN ${CMAKE_BINARY_DIR}/${variant_name}.bin)
  set(LANG_FINAL_BIN ${CMAKE_CURRENT_BINARY_DIR}/lang/${variant_name}_lang.bin)
  set(LANG_FINAL_HEX ${CMAKE_CURRENT_BINARY_DIR}/lang/${variant_name}_lang.hex)

  add_custom_command(OUTPUT ${LANG_FWBIN}
    COMMAND "${CMAKE_OBJCOPY}" -I ihex -O binary ${CMAKE_BINARY_DIR}/${variant_name}.hex ${LANG_FWBIN}
    DEPENDS ${variant_name}
  )
  add_custom_command(OUTPUT ${LANG_MAP}
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/lang/lang-map.py "${CMAKE_BINARY_DIR}/${variant_name}" "${LANG_FWBIN}" > "${LANG_MAP}"
    DEPENDS ${LANG_FWBIN}
  )

  set(LANG_BINS "")
  foreach (LANG IN LISTS LANG_VARIANTS)
    set(LANG_BIN ${CMAKE_CURRENT_BINARY_DIR}/lang/${variant_name}_${LANG}.bin)

    set(PO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/lang/po/Firmware_${LANG}.po")
    add_custom_command(OUTPUT ${LANG_BIN}
  #      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/lang/lang-check.py --no-warning --map "${LANG_MAP}" "${PO_FILE}"
  #      COMMAND ${CMAKE_COMMAND} -E echo "Building lang_${LANG}.bin"
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/lang/lang-build.py ${LANG_MAP} ${PO_FILE} ${LANG_BIN}
        DEPENDS ${LANG_MAP}
        COMMENT "Generating ${variant_name}_${LANG}.bin from .po"
        )
        LIST(APPEND LANG_BINS ${LANG_BIN})
  endforeach()
  add_custom_command( OUTPUT ${LANG_FINAL_BIN}
    # TODO - needs differentiation for platforms, e.g. copy /b on Win
    COMMAND cat ${LANG_BINS} > ${LANG_FINAL_BIN}
    DEPENDS ${LANG_BINS}
    COMMENT "Merging language binaries"
  )
  add_custom_command( OUTPUT ${LANG_FINAL_HEX}
  # TODO - needs differentiation for platforms, e.g. copy /b on Win
    COMMAND ${CMAKE_OBJCOPY} -I binary -O ihex ${LANG_FINAL_BIN} ${LANG_FINAL_HEX}
    DEPENDS ${LANG_FINAL_BIN}
    COMMENT "Generating Hex for language data"
  )
  set(LANG_HEX ${CMAKE_BINARY_DIR}/${variant_name}-lang.hex)
  add_custom_target(${variant_name}-languages
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${variant_name}.hex ${LANG_HEX}
    COMMAND cat ${LANG_FINAL_HEX} >> ${LANG_HEX}
    COMMENT "Generating final ${variant_name}-lang.hex"
    BYPRODUCTS ${LANG_HEX}
    DEPENDS ${LANG_FINAL_HEX}
  )

endfunction()


if(CMAKE_CROSSCOMPILING)

  add_custom_target(All_Firmware)

  file(GLOB FW_VARIANTS RELATIVE ${PROJECT_SOURCE_DIR}/Firmware/variants ${PROJECT_SOURCE_DIR}/Firmware/variants/*.h)
  foreach(THIS_VAR IN LISTS FW_VARIANTS)
    string(REPLACE ".h" "" TRIMMED_NAME "${THIS_VAR}")
    message("Variant added: ${TRIMMED_NAME}")
    fw_add_variant(${TRIMMED_NAME})
    add_dependencies(All_Firmware ${TRIMMED_NAME})

  endforeach(THIS_VAR IN LISTS FW_VARIANTS)

endif()

if(NOT CMAKE_CROSSCOMPILING)
  # do not build the firmware by default (tests are the focus if not crosscompiling)
  project(cmake_test)

  # Prepare "Catch" library for other executables
  set(CATCH_INCLUDE_DIR Catch2)
  add_library(Catch INTERFACE)
  target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

  # Make test executable
  set(TEST_SOURCES
    Tests/tests.cpp
    Tests/Example_test.cpp
    Tests/Timer_test.cpp
    Tests/AutoDeplete_test.cpp
    Tests/PrusaStatistics_test.cpp
    Firmware/Timer.cpp
    Firmware/AutoDeplete.cpp
  )
  add_executable(tests ${TEST_SOURCES})
  target_include_directories(tests PRIVATE Tests)
  target_link_libraries(tests Catch)

endif()
