if(DEFINED TOOLCHAIN_PREFIX)
  # subsequent toolchain loading is not really needed
  return()
endif()

# target triplet
if(CMAKE_SYSTEM_PROCESSOR MATCHES riscv)
  set(TOOLCHAIN_PREFIX riscv64-${VENDOR}${MODE})
else()
  set(TOOLCHAIN_PREFIX ${CMAKE_SYSTEM_PROCESSOR}-${VENDOR}${MODE})
endif()

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
  set(EXE ".exe")
  set(NULL_DEV "NUL")
else()
  set(NULL_DEV "/dev/null")
endif()

find_program(GCC_EXE ${TOOLCHAIN_PREFIX}-gcc${EXE} HINTS ENV PATH REQUIRED)

get_filename_component(CROSS_PATH ${GCC_EXE} DIRECTORY)
get_filename_component(CROSS_PATH ${CROSS_PATH} DIRECTORY)

# setup correct sysroot path
execute_process(
  COMMAND ${GCC_EXE} --print-sysroot
  OUTPUT_STRIP_TRAILING_WHITESPACE
  OUTPUT_VARIABLE SYSROOT_DIR)

if(NOT SYSROOT_DIR STREQUAL "")
  get_filename_component(SYSROOT_DIR ${SYSROOT_DIR} ABSOLUTE)

  set(CMAKE_FIND_ROOT_PATH ${SYSROOT_DIR})
  set(CMAKE_SYSROOT ${SYSROOT_DIR})
endif()

set(CMAKE_C_COMPILER_EXTERNAL_TOOLCHAIN ${CROSS_PATH})
set(CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN ${CROSS_PATH})

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

# GNU C compiler
set(CMAKE_C_COMPILER
    ${CROSS_PATH}/bin/${TOOLCHAIN_PREFIX}-gcc${EXE}
    CACHE PATH "C compiler")
set(CMAKE_CXX_COMPILER
    ${CROSS_PATH}/bin/${TOOLCHAIN_PREFIX}-g++${EXE}
    CACHE PATH "C++ compiler")
set(CMAKE_ASM_COMPILER
    ${CROSS_PATH}/bin/${TOOLCHAIN_PREFIX}-gcc${EXE}
    CACHE PATH "ASM compiler")

set(CMAKE_OBJDUMP
    ${CROSS_PATH}/bin/${TOOLCHAIN_PREFIX}-objdump${EXE}
    CACHE STRING "Objdump path")
set(CMAKE_OBJCOPY
    ${CROSS_PATH}/bin/${TOOLCHAIN_PREFIX}-objcopy${EXE}
    CACHE STRING "Objcopy path")

if(DEFINED MARCH)
  set(TARGET_MARCH ${MARCH})
endif()

if(DEFINED MABI)
  set(TARGET_MABI ${MABI})
endif()

if(DEFINED MTUNE)
  set(TARGET_MTUNE ${MTUNE})
endif()

if(DEFINED CPU AND DEFINED PLATFORM)
  message(FATAL_ERROR "CPU and PLATFORM cannot be defined simultaneously")
endif()

# CPU tunings
if(DEFINED CPU)
  include("${CMAKE_CURRENT_LIST_DIR}/cpu/${CPU}.cmake")
  message(STATUS "Apply CPU tunings for '${CPU}'")
  set(CMAKE_TRY_COMPILE_PLATFORM_VARIABLES CPU)
endif()

if(NOT DEFINED TARGET_MARCH)
  set(TARGET_MARCH ${MARCH})
endif()

if(NOT DEFINED TARGET_MABI)
  set(TARGET_MABI ${MABI})
endif()

if(NOT DEFINED TARGET_MTUNE)
  set(TARGET_MTUNE ${MTUNE})
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES riscv)
  if(ENABLE_RVV AND NOT TARGET_MARCH MATCHES "rv(32|64)[^_]*v[^ ]*")
    string(REGEX REPLACE "rv(32|64)([^_]*)([^ ]*)" "rv\\1\\2v\\3" TARGET_MARCH
                         "${TARGET_MARCH}")
  endif()

  # check riscv extensions support
  foreach(extension ${MARCH_OPTIONAL})
    if(DEFINED CMAKE_C_COMPILER_TARGET)
      set(TARGET_FLAG "--target=${CMAKE_C_COMPILER_TARGET}")
    endif()
    execute_process(
      COMMAND
        ${CMAKE_C_COMPILER} ${TARGET_FLAG} -march=${TARGET_MARCH}_${extension}
        -mabi=${MABI} -nostdlib -x c ${NULL_DEV}
      RESULT_VARIABLE ERR
      OUTPUT_QUIET ERROR_QUIET)
    if(ERR)
      message(STATUS "Checking riscv ${extension} extension support - no")
    else()
      message(STATUS "Checking riscv ${extension} extension support - yes")
      string(APPEND TARGET_MARCH _${extension})
    endif()
  endforeach()
  string(APPEND COMMON_CFLAGS " -mcmodel=medany -msmall-data-limit=8"
         " -ffunction-sections -fdata-sections"
         " -fno-common -ftls-model=local-exec")
endif()

if(TARGET_MARCH)
  string(APPEND TARGET_ARCH " -march=${TARGET_MARCH}")
endif()

if(TARGET_MABI)
  string(APPEND TARGET_ARCH " -mabi=${TARGET_MABI}")
endif()

if(TARGET_MTUNE)
  # -mtune=scrX is not supported for riscv/gcc
  if(NOT (CMAKE_SYSTEM_PROCESSOR MATCHES riscv AND CMAKE_CXX_COMPILER_ID MATCHES
                                                   GNU))
    string(APPEND TARGET_ARCH " -mtune=${TARGET_MTUNE}")
  endif()
endif()

string(STRIP "${TARGET_ARCH} ${COMMON_CFLAGS}" TARGET_FLAGS)
string(REPLACE "  " " " TARGET_FLAGS "${TARGET_FLAGS}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${TARGET_FLAGS}")

message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_SYSROOT: ${CMAKE_SYSROOT}")
message(STATUS "CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
if(CMAKE_CXX_COMPILER_ID MATCHES Clang)
  message(STATUS "CMAKE_C_COMPILER_TARGET: ${CMAKE_C_COMPILER_TARGET}")
endif()
