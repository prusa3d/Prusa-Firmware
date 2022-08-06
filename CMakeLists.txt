cmake_minimum_required(VERSION 3.15)
include(cmake/Utilities.cmake)
include(cmake/GetGitRevisionDescription.cmake)

#set (CMAKE_CXX_STANDARD 17)

SET(PROJECT_VERSION_SUFFIX
    "<auto>"
    CACHE
      STRING
      "Full version suffix to be shown on the info screen in settings (e.g. full_version=4.0.3-BETA+1035.PR111.B4, suffix=-BETA+1035.PR111.B4). Defaults to '+<commit sha>.<dirty?>.<debug?>' if set to '<auto>'."
    )
SET(PROJECT_VERSION_SUFFIX_SHORT
    "<auto>"
    CACHE
      STRING
      "Short version suffix to be shown on splash screen. Defaults to '+<BUILD_NUMBER>' if set to '<auto>'."
    )
SET(BUILD_NUMBER
    ""
    CACHE STRING "Build number of the firmware. Resolved automatically if not specified."
    )


INCLUDE(cmake/ProjectVersion.cmake)
resolve_version_variables()


SET(PROJECT_VERSION_FLAVOUR "" CACHE STRING "Firmware flavour to build - DEBUG, DEVEL, APLHA, BETA or RC")
SET(PROJECT_VERSION_FLAVOUR_REVISION "" CACHE STRING "Firmware flavour version, e.g. 1 for RC1, etc")


IF( NOT PROJECT_VERSION_FLAVOUR STREQUAL "")
    SET(PROJECT_VERSION "${PROJECT_VERSION}-${PROJECT_VERSION_FLAVOUR}")
    add_compile_definitions(FW_FLAVOR=${PROJECT_VERSION_FLAVOUR})
    IF( NOT PROJECT_VERSION_FLAVOUR_REVISION STREQUAL "")
        SET(PROJECT_VERSION "${PROJECT_VERSION}${PROJECT_VERSION_FLAVOUR_REVISION}")
        add_compile_definitions(FW_FLAVERSION=${PROJECT_VERSION_FLAVOUR_REVISION})
    ENDIF()
ENDIF()

# Inform user about the resolved settings
MESSAGE(STATUS "Project version: ${PROJECT_VERSION}")
MESSAGE(
  STATUS "Project version with short suffix: ${PROJECT_VERSION}${PROJECT_VERSION_SUFFIX_SHORT}"
  )

SET(FN_PREFIX "FW${PROJECT_VERSION}${PROJECT_VERSION_SUFFIX_SHORT}")

MESSAGE(WARNING     "
***************** YOUR ATTENTION PLEASE *****************
CMake support is experimental. There is no guarantee at this time. If you have problems you are encouraged to fall back to the tried-and-true methods.
*********************** THANK YOU **********************
We now return to your regularly scheduled Firmware Build."
)

OPTION(SECONDARY_LANGUAGES "Secondary language support in the firmware" ON)

SET(MAIN_LANGUAGES cs de es fr it pl CACHE STRING "The list of 'main' languages to be included, in the correct order")
SET(COMMUNITY_LANGUAGES nl ro hu hr sk sv no CACHE STRING "The list of community languages to be included, in the correct order")
SET(SELECTED_LANGUAGES ${MAIN_LANGUAGES} ${COMMUNITY_LANGUAGES})

get_dependency_directory(prusa3dboards PRUSA_BOARDS_DIR)
project(Prusa-Firmware)

FILE(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/Firmware/config.h MAX_SIZE_LINE REGEX "^#define \+LANG_SIZE_RESERVED \+")
STRING(REGEX MATCH "0x[0-9]+" MAX_SIZE_HEX "${MAX_SIZE_LINE}")
math(EXPR LANG_MAX_SIZE "${MAX_SIZE_HEX}" OUTPUT_FORMAT DECIMAL)
message("Language maximum size (from config.h): ${LANG_MAX_SIZE} bytes")

set (LANG_BIN_MAX 249856) # Ditto, this in xflash_layout.h but needs invocation of the preprocessor... :-/

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

#
# Firmware - get file lists.
#
SET(FW_SOURCES
    adc.c
    bootapp.c
    timer02.c
    sm4.c
    spi.c
    rbuf.c
    swi2c.c
    language.c
    tone04.c
    uart2.c
    xflash.c
    Marlin_main.cpp
    AutoDeplete.cpp
    Configuration.cpp
    BlinkM.cpp
    Dcodes.cpp
    MarlinSerial.cpp
    ConfigurationStore.cpp
    Sd2Card.cpp
    SdBaseFile.cpp
    SdFatUtil.cpp
    SdFile.cpp
    SdVolume.cpp
    Servo.cpp
    Timer.cpp
    backlight.cpp
    cardreader.cpp
    cmdqueue.cpp
    conv2str.cpp
    eeprom.cpp
    first_lay_cal.cpp
    fsensor.cpp
    heatbed_pwm.cpp
    la10compat.cpp
    lcd.cpp
    menu.cpp
    mesh_bed_calibration.cpp
    mesh_bed_leveling.cpp
    messages.cpp
    mmu.cpp
    motion_control.cpp
    optiboot_xflash.cpp
    pat9125.cpp
    planner.cpp
    qr_solve.cpp
    sound.cpp
    speed_lookuptable.cpp
    stepper.cpp
    swspi.cpp
    temperature.cpp
    tmc2130.cpp
    twi.cpp
    ultralcd.cpp
    util.cpp
    vector_3.cpp
    xflash_dump.cpp
    xyzcal.cpp
)
list(TRANSFORM FW_SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/Firmware/)

foreach(_FILE ${FW_SOURCES})
    get_filename_component(_BASE ${_FILE} NAME)
    set_property(SOURCE ${_FILE} APPEND_STRING PROPERTY COMPILE_FLAGS "-frandom-seed=${_BASE}.o")
endforeach()


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
#   new.cpp # What happened to this? it was removed in 1.0.5-1 to 1.0.5.2?
)
list(TRANSFORM AVR_SOURCES PREPEND ${PRUSA_BOARDS_DIR}/cores/prusa_einsy_rambo/)

foreach(_FILE ${AVR_SOURCES})
    get_filename_component(_BASE ${_FILE} NAME)
    set_property(SOURCE ${_FILE} APPEND_STRING PROPERTY COMPILE_FLAGS "-frandom-seed=core/${_BASE}.o")
endforeach()

# optimizations
if(CMAKE_CROSSCOMPILING)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_compile_options(-Og)
	else()
		add_compile_options(-Os)
	endif()

	# mcu related settings
	set(MCU_FLAGS -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10819 -DARDUINO_AVR_PRUSA_EINSY_RAMBO -DARDUINO_ARCH_AVR)
	add_compile_options(${MCU_FLAGS})

    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
    add_compile_options(-Wall -Wextra -Wno-expansion-to-defined -ffunction-sections -fdata-sections -MMD -flto -fno-fat-lto-objects)

    # split and gc sections
    add_link_options(-Os -g -flto -Wl,--gc-sections -mmcu=atmega2560 -Wl,-u,vfprintf -lprintf_flt -lm )

    # Create this target before we apply the GC options
    add_library(avr_core STATIC ${AVR_SOURCES})
    target_include_directories(avr_core PRIVATE
        ${PRUSA_BOARDS_DIR}/cores/prusa_einsy_rambo/
        ${PRUSA_BOARDS_DIR}/variants/prusa_einsy_rambo/
        )



	# disable exceptions and related metadata
	add_compile_options(-fno-unwind-tables)
	add_link_options(-Wl,--defsym,__exidx_start=0,--defsym,__exidx_end=0)
else()
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		add_compile_options(-O0)
	else()
		add_compile_options(-O2)
	endif()
endif()

# enable all warnings (well, not all, but some)
add_compile_options(-Wsign-compare)
add_compile_options($<$<COMPILE_LANGUAGE:C>:-std=gnu11>)

# support _DEBUG macro (some code uses to recognize debug builds)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_definitions(_DEBUG)
endif()



# Setup language resources:
# file(GLOB LANG_VARIANTS RELATIVE ${PROJECT_SOURCE_DIR}/lang/po ${PROJECT_SOURCE_DIR}/lang/po/Firmware_??.po)
# string(REPLACE "Firmware_" "" LANG_VARIANTS "${LANG_VARIANTS}")
# string(REPLACE ".po" "" LANG_VARIANTS "${LANG_VARIANTS}")
# list(SORT LANG_VARIANTS)
# message("Languages found: ${LANG_VARIANTS}")


# Meta target to build absolutely everything
add_custom_target(ALL_FIRMWARE)

function(fw_add_variant variant_name)

	# Create the Configuration_Prusa.h for this variant so it can be #included.
	set(VARIANT_CFG_DIR "${CMAKE_CURRENT_BINARY_DIR}/${variant_name}_include")
	set(VARIANT_CFG_FILE "${VARIANT_CFG_DIR}/Configuration_prusa.h")
	add_custom_command(OUTPUT ${VARIANT_CFG_FILE}
		COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/Firmware/variants/${variant_name}.h ${VARIANT_CFG_FILE}
		COMMENT "Generating Configuration_prusa.h for ${variant_name}"
		BYPRODUCTS ${VARIANT_CFG_DIR}
	)
	STRING(REPLACE "1_75mm_" "" variant_name "${variant_name}")
	STRING(REPLACE "-E3Dv6full" "" variant_name "${variant_name}")

	add_executable(${variant_name} ${FW_SOURCES} ${FW_HEADERS} ${VARIANT_CFG_FILE})

	set_target_properties(${variant_name} PROPERTIES CXX_STANDARD 17)

	target_include_directories(${variant_name} PRIVATE Firmware
		${PRUSA_BOARDS_DIR}/cores/prusa_einsy_rambo/
		${PRUSA_BOARDS_DIR}/variants/prusa_einsy_rambo/
		${VARIANT_CFG_DIR} # Include the header for this variant.
	)

    target_link_libraries(${variant_name} avr_core)

#   # configure linker script
	 set(LINKER_SCRIPT ${PRUSA_BOARDS_DIR}/ldscripts/avr6.xn)
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

	target_compile_options(${variant_name} PRIVATE) # turn this on for lolz -Wdouble-promotion)
	target_compile_definitions(${variant_name} PRIVATE
		CMAKE_LANG_CONTROL
	)

	if (SECONDARY_LANGUAGES)
		target_compile_definitions(${variant_name} PUBLIC LANG_MODE=1)
	else()
		target_compile_definitions(${variant_name} PUBLIC LANG_MODE=0)
        add_custom_command(
            TARGET ${variant_name}
            POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -O ihex ${CMAKE_BINARY_DIR}/${variant_name} ${CMAKE_BINARY_DIR}/${FN_PREFIX}-${variant_name}-EN_ONLY.hex
            COMMENT "Generating ${variant_name} hex"
        )
		add_dependencies(ALL_FIRMWARE "${variant_name}")
		return() #Done, if no languages there's nothing else to do.
	endif()

	#Construct language map
	set(LANG_TMP_DIR ${CMAKE_CURRENT_BINARY_DIR}/lang)
	set(LANG_MAP ${LANG_TMP_DIR}/${variant_name}_lang.map)
	set(LANG_FWBIN ${CMAKE_BINARY_DIR}/${variant_name}.bin)
	set(LANG_FINAL_BIN ${LANG_TMP_DIR}/${variant_name}_lang.bin)
	set(LANG_FINAL_HEX ${LANG_TMP_DIR}/${variant_name}_lang.hex)

	add_custom_command(OUTPUT ${LANG_FWBIN}
		COMMAND "${CMAKE_OBJCOPY}" -I ihex -O binary ${CMAKE_BINARY_DIR}/${variant_name}.hex ${LANG_FWBIN}
		DEPENDS ${variant_name}
	)
	add_custom_command(OUTPUT ${LANG_MAP}
		COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/lang/lang-map.py "${CMAKE_BINARY_DIR}/${variant_name}" "${LANG_FWBIN}" > "${LANG_MAP}"
		DEPENDS ${LANG_FWBIN}
	)

	set(LANG_BINS "")

	foreach (LANG IN LISTS SELECTED_LANGUAGES)
		set(LANG_BIN ${LANG_TMP_DIR}/${variant_name}_${LANG}.bin)

		set(PO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/lang/po/Firmware_${LANG}.po")
		add_custom_command(OUTPUT ${LANG_BIN}
	#      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/lang/lang-check.py --no-warning --map "${LANG_MAP}" "${PO_FILE}"
	#      COMMAND ${CMAKE_COMMAND} -E echo "Building lang_${LANG}.bin"
			COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/lang/lang-build.py ${LANG_MAP} ${PO_FILE} ${LANG_BIN}
			# Check bin size:
			COMMAND ${CMAKE_COMMAND} -DLANG_MAX_SIZE=${LANG_MAX_SIZE}
				-DLANG_FILE=${LANG_BIN}
				-P ${PROJECT_CMAKE_DIR}/Check_lang_size.cmake
			DEPENDS ${LANG_MAP}
			COMMENT "Generating ${variant_name}_${LANG}.bin from .po"
		)
		LIST(APPEND LANG_BINS ${LANG_BIN})

	endforeach()
	string(FIND ${variant_name} "MK3" HAS_XFLASH)
	if (${HAS_XFLASH} GREATER_EQUAL 0)
        add_custom_command( OUTPUT ${LANG_FINAL_BIN}
            COMMAND ${CMAKE_COMMAND} -E cat ${LANG_BINS} > ${LANG_FINAL_BIN}
            DEPENDS ${LANG_BINS}
            COMMENT "Merging language binaries (Non-W32)"
        )
		add_custom_command( OUTPUT ${LANG_FINAL_BIN}
			COMMAND ${CMAKE_COMMAND} -DLANG_MAX_SIZE=${LANG_BIN_MAX}
				-DLANG_FILE=${LANG_FINAL_BIN}
				-P ${PROJECT_CMAKE_DIR}/Check_final_lang_bin_size.cmake
			APPEND
		)
		add_custom_command( OUTPUT ${LANG_FINAL_HEX}
			COMMAND ${CMAKE_OBJCOPY} -I binary -O ihex ${LANG_FINAL_BIN} ${LANG_FINAL_HEX}
			DEPENDS ${LANG_FINAL_BIN}
			COMMENT "Generating Hex for language data"
		)
		set(LANG_HEX ${CMAKE_BINARY_DIR}/${FN_PREFIX}-${variant_name}-lang.hex)

		add_custom_target(${variant_name}-languages
			COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${variant_name}.hex ${LANG_HEX}
			COMMAND ${CMAKE_COMMAND} -E cat ${LANG_FINAL_HEX} >> ${LANG_HEX}
			COMMENT "Generating final ${variant_name}-lang.hex"
			BYPRODUCTS ${LANG_HEX}
			DEPENDS ${LANG_FINAL_HEX}
		)
		add_dependencies(ALL_FIRMWARE ${variant_name}-languages)
	else()
		set (ALL_VARIANT_HEXES "")
		# Non-xflash, e.g. MK2.5
		foreach(LANG IN LISTS SELECTED_LANGUAGES)
			SET(LANG_HEX_FN ${variant_name}-en_${LANG})
			SET(LANG_HEX ${CMAKE_BINARY_DIR}/${FN_PREFIX}-${LANG_HEX_FN}.hex)
			SET(LANG_BIN ${LANG_TMP_DIR}/${variant_name}_${LANG}.bin)
			SET(LANG_FWBIN_TMP ${LANG_TMP_DIR}/${variant_name}-en_${LANG}.bin)

			#Intermediate 2-lang bin
			add_custom_command(OUTPUT ${LANG_FWBIN_TMP}
				COMMAND ${CMAKE_COMMAND} -E copy ${LANG_FWBIN} ${LANG_FWBIN_TMP}
				COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/lang/lang-patchsec.py ${CMAKE_BINARY_DIR}/${variant_name} ${LANG_BIN} ${LANG_FWBIN_TMP}
				DEPENDS ${LANG_FWBIN} ${LANG_BIN}
				COMMENT "Generating ${variant_name}-en_${LANG}.bin"
			)

			#Final hex:
			add_custom_target(${LANG_HEX_FN}
				COMMAND ${CMAKE_OBJCOPY} -I binary -O ihex ${LANG_FWBIN_TMP} ${LANG_HEX}
				BYPRODUCTS ${LANG_HEX}
				DEPENDS ${LANG_FWBIN_TMP}
				COMMENT "Creating ${LANG_HEX_FN}.hex"
			)
			LIST(APPEND ALL_VARIANT_HEXES ${LANG_HEX_FN})
		endforeach()
		add_custom_target("${variant_name}-All-Languages"
			DEPENDS ${ALL_VARIANT_HEXES}
		)
		add_dependencies(ALL_FIRMWARE "${variant_name}-All-Languages")
	endif()
endfunction()


if(CMAKE_CROSSCOMPILING)

	file(GLOB FW_VARIANTS RELATIVE ${PROJECT_SOURCE_DIR}/Firmware/variants ${PROJECT_SOURCE_DIR}/Firmware/variants/*.h)
	foreach(THIS_VAR IN LISTS FW_VARIANTS)
		string(REPLACE ".h" "" TRIMMED_NAME "${THIS_VAR}")
		message("Variant added: ${TRIMMED_NAME}")
		fw_add_variant(${TRIMMED_NAME})
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
