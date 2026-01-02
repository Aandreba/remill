find_package(Python3 COMPONENTS Interpreter REQUIRED)
message(STATUS "Python3: ${Python3_EXECUTABLE}")

# Reference: https://github.com/lifting-bits/cxx-common/blob/e0063b2f5986582ed8dcab0c2863abf0893b3082/ports/xed/portfile.cmake

# TODO: pass compiler flags

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND MSVC)
    set(compiler ms) #msvc or clang-cl
elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang$")
    set(compiler clang)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(compiler gnu)
else()
    message(FATAL_ERROR "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

set(MFILE_ARGS
    "install"
    "--install-dir=install"
    "--cc=${CMAKE_C_COMPILER}"
    "--cxx=${CMAKE_CXX_COMPILER}"
    "--compiler=${compiler}"
)

if(CMAKE_OSX_SYSROOT)
    list(APPEND MFILE_ARGS "--extra-ccflags=-isysroot ${CMAKE_OSX_SYSROOT}")
    list(APPEND MFILE_ARGS "--extra-cxxflags=-isysroot ${CMAKE_OSX_SYSROOT}")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    list(APPEND MFILE_ARGS "--extra-ccflags=${ADDITIONAL_FLAGS}")
    list(APPEND MFILE_ARGS "--extra-cxxflags=${ADDITIONAL_FLAGS}")
endif()

if(USE_SANITIZERS)
    list(APPEND MFILE_ARGS "--extra-ccflags=-fsanitize=address,undefined")
    list(APPEND MFILE_ARGS "--extra-cxxflags=-fsanitize=address,undefined")
endif()

if(BUILD_SHARED_LIBS)
    list(APPEND MFILE_ARGS "--shared")
else()
    list(APPEND MFILE_ARGS "--static")
endif()

if(CMAKE_AR)
    list(APPEND MFILE_ARGS "--ar=${CMAKE_AR}")
endif()

ExternalProject_Add(mbuild
    GIT_REPOSITORY
        "https://github.com/intelxed/mbuild"
    GIT_TAG
        "v2024.11.04"
    GIT_PROGRESS
        ON
    CONFIGURE_COMMAND
        "${CMAKE_COMMAND}" -E true
    BUILD_COMMAND
        "${CMAKE_COMMAND}" -E true
    INSTALL_COMMAND
        "${CMAKE_COMMAND}" -E true
    PREFIX
        xed-prefix
)

ExternalProject_Add(xed
    GIT_REPOSITORY
        "https://github.com/intelxed/xed"
    GIT_TAG
        "v2025.06.08"
    GIT_PROGRESS
        ON
    CMAKE_CACHE_ARGS
        ${CMAKE_ARGS}
    CONFIGURE_COMMAND
        "${CMAKE_COMMAND}" -E true
    BUILD_COMMAND
        "${Python3_EXECUTABLE}" "<SOURCE_DIR>/mfile.py" ${MFILE_ARGS}
    INSTALL_COMMAND
        "${CMAKE_COMMAND}" -E true
    PREFIX
        xed-prefix
)

ExternalProject_Get_Property(xed BINARY_DIR)
cmake_path(APPEND BINARY_DIR "install" "include" OUTPUT_VARIABLE xed_include_path)
if (MSVC)
    cmake_path(APPEND BINARY_DIR "install" "lib" "xed.lib" OUTPUT_VARIABLE xed_xed_path)
    cmake_path(APPEND BINARY_DIR "install" "lib" "xed-ild.lib" OUTPUT_VARIABLE xed_ild_path)
else()
    cmake_path(APPEND BINARY_DIR "install" "lib" "xed.a" OUTPUT_VARIABLE xed_xed_path)
    cmake_path(APPEND BINARY_DIR "install" "lib" "xed-ild.a" OUTPUT_VARIABLE xed_ild_path)
endif()

message(STATUS "XED Include Path: ${xed_include_path}")
message(STATUS "XED Lib Path: ${xed_xed_path}")
message(STATUS "XED IDL Lib Path: ${xed_ild_path}")

add_library(XED INTERFACE)
target_link_libraries(XED INTERFACE ${xed_xed_path} ${xed_ild_path})
target_include_directories(XED INTERFACE ${xed_include_path})

# TODO: generate XEDVersion.cmake as well file
# configure_file("${CMAKE_CURRENT_SOURCE_DIR}/XEDConfig.cmake.in" "${CMAKE_INSTALL_PREFIX}/lib/cmake/XED/XEDConfig.cmake" @ONLY)
