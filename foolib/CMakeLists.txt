# CMake file by +KZ
# may not have all options that .xml files had
cmake_minimum_required(VERSION 4.4)

project(foolib)

# Libraries

## SDL3
find_package(SDL3 REQUIRED CONFIG)

## OpenGL
find_package(OpenGL REQUIRED)

set(FOOLIB_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source/foo)

file(GLOB FOOLIB_SRC CONFIGURE_DEPENDS
    ${FOOLIB_SRC_DIR}/base/*.cpp
    ${FOOLIB_SRC_DIR}/base/*.h
    ${FOOLIB_SRC_DIR}/core/*.cpp
    ${FOOLIB_SRC_DIR}/core/*.h
    ${FOOLIB_SRC_DIR}/ext/3d/*.cpp
    ${FOOLIB_SRC_DIR}/ext/3d/*.h
    ${FOOLIB_SRC_DIR}/ext/3d/2dutils/*.cpp
    ${FOOLIB_SRC_DIR}/ext/3d/2dutils/*.h
    ${FOOLIB_SRC_DIR}/ext/3d/opengl/*.cpp
    ${FOOLIB_SRC_DIR}/ext/3d/opengl/*.h
    ${FOOLIB_SRC_DIR}/ext/compr/*.cpp
    ${FOOLIB_SRC_DIR}/ext/compr/*.h
    ${FOOLIB_SRC_DIR}/ext/gui_new/*.cpp
    ${FOOLIB_SRC_DIR}/ext/gui_new/*.h
    ${FOOLIB_SRC_DIR}/ext/input/*.cpp
    ${FOOLIB_SRC_DIR}/ext/input/*.h
    ${FOOLIB_SRC_DIR}/ext/loader/*.cpp
    ${FOOLIB_SRC_DIR}/ext/loader/*.h
    ${FOOLIB_SRC_DIR}/ext/loader/jpeg/*.cpp
    ${FOOLIB_SRC_DIR}/ext/loader/jpeg/*.h
    ${FOOLIB_SRC_DIR}/ext/loader/png/*.cpp
    ${FOOLIB_SRC_DIR}/ext/loader/png/*.h
    ${FOOLIB_SRC_DIR}/ext/loader/tga/*.cpp
    ${FOOLIB_SRC_DIR}/ext/loader/tga/*.h
    ${FOOLIB_SRC_DIR}/ext/network/*.cpp
    ${FOOLIB_SRC_DIR}/ext/network/*.h
    ${FOOLIB_SRC_DIR}/ext/network/slotserver/*.cpp
    ${FOOLIB_SRC_DIR}/ext/network/slotserver/*.h
    ${FOOLIB_SRC_DIR}/ext/sound/*.cpp
    ${FOOLIB_SRC_DIR}/ext/sound/*.h
    ${FOOLIB_SRC_DIR}/ext/sound/devices/*.cpp
    ${FOOLIB_SRC_DIR}/ext/sound/devices/*.h
    ${FOOLIB_SRC_DIR}/ext/sound/sources/*.cpp
    ${FOOLIB_SRC_DIR}/ext/sound/sources/*.h
    ${FOOLIB_SRC_DIR}/func/*.cpp
    ${FOOLIB_SRC_DIR}/func/*.h
)

add_library(foolib STATIC ${FOOLIB_SRC})

target_include_directories(foolib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)

target_link_libraries(foolib
    PUBLIC
        SDL3::SDL3
        OpenGL::GL
        OpenGL::GLU
)

# Always defined
target_compile_definitions(foolib PUBLIC FOO_LOWLEVEL_STRING_LIBC)
target_compile_definitions(foolib PUBLIC FOO_LOWLEVEL_MEMORY_LIBC)
target_compile_definitions(foolib PUBLIC FOO_LOWLEVEL_MATH_LIBC)
target_compile_definitions(foolib PUBLIC FOO_MODE_DEBUG) # for now i guess

# Compiler
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    target_compile_definitions(foolib PUBLIC FOO_COMPILER_CLANG)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_definitions(foolib PUBLIC FOO_COMPILER_GCC)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_definitions(foolib PUBLIC FOO_COMPILER_MSVC7)
endif()


# CPU Architecture
if(CMAKE_SYSTEM_PROCESSOR MATCHES "i386|i686|x86")
    target_compile_definitions(foolib PUBLIC FOO_CPU_X86)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    target_compile_definitions(foolib PUBLIC FOO_CPU_ARM64)
endif()


# Platform
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(foolib PUBLIC FOO_PLATFORM_WIN32)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_compile_definitions(foolib PUBLIC FOO_PLATFORM_DARWIN)
    target_compile_definitions(foolib PUBLIC FOO_PLATFORM_POSIX)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(foolib PUBLIC FOO_PLATFORM_LINUX)
    target_compile_definitions(foolib PUBLIC FOO_PLATFORM_POSIX)
endif()
