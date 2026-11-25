# CMake file by +KZ
cmake_minimum_required(VERSION 4.4)

project(Teepop)

# Libraries

## foolib
add_subdirectory(foolib)

## Use pkg-config to find other libs
find_package(PkgConfig REQUIRED)

pkg_check_modules(VORBIS REQUIRED IMPORTED_TARGET vorbisfile)

# Teepop sources
set(TEEPOP_ENGINE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/teepop/engine/src/engine)
file(GLOB TEEPOP_ENGINE_SRC CONFIGURE_DEPENDS
    ${TEEPOP_ENGINE_SRC_DIR}/*.cpp
    ${TEEPOP_ENGINE_SRC_DIR}/*.h
)

set(TEEPOP_SHARED_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/teepop/engine/src/shared)
file(GLOB TEEPOP_SHARED_SRC CONFIGURE_DEPENDS
    ${TEEPOP_SHARED_SRC_DIR}/*.cpp
    ${TEEPOP_SHARED_SRC_DIR}/*.h
)

set(TEEPOP_CLIENT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/teepop/engine/src/client)
file(GLOB TEEPOP_CLIENT_SRC CONFIGURE_DEPENDS
    ${TEEPOP_CLIENT_SRC_DIR}/*.cpp
    ${TEEPOP_CLIENT_SRC_DIR}/*.h
)

set(TEEPOP_SERVER_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/teepop/engine/src/server)
file(GLOB TEEPOP_SERVER_SRC CONFIGURE_DEPENDS
    ${TEEPOP_SERVER_SRC_DIR}/*.cpp
    ${TEEPOP_SERVER_SRC_DIR}/*.h
)

add_executable(Teepop_client ${TEEPOP_ENGINE_SRC} ${TEEPOP_SHARED_SRC} ${TEEPOP_CLIENT_SRC})
add_executable(Teepop_server ${TEEPOP_ENGINE_SRC} ${TEEPOP_SHARED_SRC} ${TEEPOP_SERVER_SRC})

target_link_libraries(Teepop_client
    PRIVATE
        "$<LINK_LIBRARY:WHOLE_ARCHIVE,foolib>"
        PkgConfig::VORBIS
)

target_link_libraries(Teepop_server
    PRIVATE
        "$<LINK_LIBRARY:WHOLE_ARCHIVE,foolib>"
)

# copy data
add_custom_target(TEEPOP_DATA ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/teepop/engine/data/ ${CMAKE_BINARY_DIR}/data/
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/teepop/engine/config.registry ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/teepop/engine/users.cfg ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/teepop/engine/readme.txt ${CMAKE_BINARY_DIR}/
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/teepop/engine/changelog.txt ${CMAKE_BINARY_DIR}/
)
