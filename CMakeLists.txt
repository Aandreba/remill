# Copyright (c) 2018 Trail of Bits, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.21)

# Setup to use ccache
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/ccache.cmake")

project(remill C CXX ASM)
include(GNUInstallDirs)
include(FetchContent)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/settings.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/utils.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/options.cmake")

if(REMILL_ENABLE_TESTING)
  include(CTest)
endif()

message(STATUS "Compiler ID is ${CMAKE_C_COMPILER_ID}")

#
# libraries
#

# LLVM
find_package(LLVM CONFIG REQUIRED)
# https://github.com/JonathanSalwan/Triton/issues/1082#issuecomment-1030826696
if(LLVM_LINK_LLVM_DYLIB)
  set(llvm_libs LLVM)
else()
  llvm_map_components_to_libnames(llvm_libs
    support core irreader
    bitreader bitwriter
    passes asmprinter
    aarch64info aarch64desc aarch64codegen aarch64asmparser
    armcodegen armasmparser
    interpreter mcjit
    nvptxdesc
    x86info x86codegen x86asmparser
    webassemblydesc)
endif()
message(STATUS "LLVM Libraries: ${llvm_libs}")

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})

string(REPLACE "." ";" LLVM_VERSION_LIST ${LLVM_PACKAGE_VERSION})
list(GET LLVM_VERSION_LIST 0 LLVM_MAJOR_VERSION)
list(GET LLVM_VERSION_LIST 1 LLVM_MINOR_VERSION)

set(LLVM_MAJOR_VERSION "${LLVM_MAJOR_VERSION}")
set(LLVM_MINOR_VERSION "${LLVM_MINOR_VERSION}")

set(REMILL_LLVM_VERSION "${LLVM_MAJOR_VERSION}")

message(STATUS "Remill LLVM version: ${REMILL_LLVM_VERSION}")
set(REMILL_INSTALL_SEMANTICS_DIR "${CMAKE_INSTALL_PREFIX}/${REMILL_INSTALL_SHARE_DIR}/remill/${REMILL_LLVM_VERSION}/semantics" CACHE PATH "Directory into which semantics are installed")
set(REMILL_BUILD_SEMANTICS_DIR_X86 "${CMAKE_CURRENT_BINARY_DIR}/lib/Arch/X86/Runtime")
set(REMILL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(REMILL_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BCCompiler.cmake")

# Intel XED
include(cmake/xed.cmake)

# Verbose fetch content updates
set(FETCHCONTENT_QUIET OFF)

# For Linux builds, group LLVM libraries into a single group
# that avoids frustrating library ordering issues.
if(UNIX AND NOT APPLE)
  set(LINKER_START_GROUP "-Wl,--start-group")
  set(LINKER_END_GROUP "-Wl,--end-group")
else()
  set(LINKER_START_GROUP "")
  set(LINKER_END_GROUP "")
endif()

#
# Configuration options for semantics
#
option(REMILL_BARRIER_AS_NOP "Remove compiler barriers (inline assembly) in semantics" OFF)
option(REMILL_BUILD_SPARC32_RUNTIME "Build the Runtime for SPARC32. Turn this off if you have include errors with <bits/c++config.h>, or read the README for a fix" OFF)

#
# target settings
#

# add everything as public.
add_library(remill_settings INTERFACE)

target_compile_features(remill_settings INTERFACE cxx_std_17)

target_include_directories(remill_settings INTERFACE
  $<BUILD_INTERFACE:${REMILL_INCLUDE_DIR}>
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${LLVM_INCLUDE_DIR}>
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND MSVC)
  # warnings and compiler settings
  target_compile_options(remill_settings INTERFACE
    "$<$<CONFIG:Debug>:/MDd>$<$<CONFIG:Release>:/MD>"
    /nologo /W3 /EHsc /wd4141 /wd4146 /wd4180 /wd4244
    /wd4258 /wd4267 /wd4291 /wd4345 /wd4351 /wd4355 /wd4456
    /wd4457 /wd4458 /wd4459 /wd4503 /wd4624 /wd4722 /wd4800
    /wd4100 /wd4127 /wd4512 /wd4505 /wd4610 /wd4510 /wd4702
    /wd4245 /wd4706 /wd4310 /wd4701 /wd4703 /wd4389 /wd4611
    /wd4805 /wd4204 /wd4577 /wd4091 /wd4592 /wd4324
  )

  target_compile_definitions(remill_settings INTERFACE
    _CRT_SECURE_NO_DEPRECATE
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_DEPRECATE
    _CRT_NONSTDC_NO_WARNINGS
    _SCL_SECURE_NO_DEPRECATE
    _SCL_SECURE_NO_WARNINGS
    GOOGLE_PROTOBUF_NO_RTTI
  )

else()
  # warnings and compiler settings
  set_target_properties(remill_settings PROPERTIES 
    INTERFACE_POSITION_INDEPENDENT_CODE ON
  )
  target_compile_options(remill_settings INTERFACE
    -Wall -Wextra -Wno-unused-parameter -Wno-c++98-compat
    -Wno-unreachable-code-return -Wno-nested-anon-types
    -Wno-extended-offsetof
    -Wno-variadic-macros -Wno-return-type-c-linkage
    -Wno-c99-extensions -Wno-ignored-attributes -Wno-unused-local-typedef
    -Wno-unknown-pragmas -Wno-unknown-warning-option
    -fno-omit-frame-pointer -fvisibility-inlines-hidden
    -fno-asynchronous-unwind-tables
  )

  # Clang-specific warnings/error options
  if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang")
    target_compile_options(remill_settings INTERFACE
      -Wgnu-alignof-expression -Wno-gnu-anonymous-struct -Wno-gnu-designator
      -Wno-gnu-zero-variadic-macro-arguments -Wno-gnu-statement-expression
      -fno-aligned-allocation
    )
  endif()

  # debug symbols
  if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    target_compile_options(remill_settings INTERFACE
      -g3
    )
  endif()

  # optimization flags and definitions
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(remill_settings INTERFACE
      "DEBUG"
    )
  else()
    target_compile_definitions(remill_settings INTERFACE
      "NDEBUG"
    )
  endif()
endif()

target_compile_definitions(remill_settings INTERFACE
  "REMILL_INSTALL_SEMANTICS_DIR=\"${REMILL_INSTALL_SEMANTICS_DIR}\""
  "REMILL_BUILD_SEMANTICS_DIR_X86=\"${REMILL_BUILD_SEMANTICS_DIR_X86}\""
  "REMILL_DISABLE_INT128"
)

target_link_libraries(remill_settings INTERFACE
  ${llvm_libs}
  XED::XED
)

add_subdirectory(lib/Arch)
add_subdirectory(lib/BC)
add_subdirectory(lib/OS)
add_subdirectory(lib/Version)

add_library(remill INTERFACE)
target_link_libraries(remill INTERFACE
  ${LINKER_START_GROUP}
  ${llvm_libs}
  XED::XED
  remill_bc
  remill_os
  remill_arch
  remill_version
  ${LINKER_END_GROUP}
)

# additional targets
#
add_custom_target(semantics)
