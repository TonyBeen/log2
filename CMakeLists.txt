cmake_minimum_required(VERSION 3.15)

project(log2 VERSION 1.0.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(LOG2_STATIC_LIB "Build static library" ON)
option(LOG2_DYNAMIC_LIB "Build dynamic library" ON)
option(ENABLE_DEBUG "Enable debug symbol" OFF)
option(USE_UNWIND "Use unwind to get stack trace" ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo") # Release MinSizeRel RelWithDebInfo Debug
endif()

if(ENABLE_DEBUG MATCHES "ON")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB LOCAL_SOURCE_FILES "src/*.cpp")
set(SOURCE_FILES ${LOCAL_SOURCE_FILES})

set(LOG_HEADER_FILES
    include/log2/log.h
    include/log2/log_def.h
)

set(CALLSTACK_HEADER_FILES
    include/callstack/callstack.h
)

add_subdirectory(example)

if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    add_compile_options(-Wall -Wextra -pthread)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND USE_UNWIND MATCHES "ON")
    add_compile_definitions(USE_UNWIND)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd)
    # link_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd/lib)
    link_libraries(${CMAKE_CURRENT_SOURCE_DIR}/3rd/lib/libunwind.a ${CMAKE_CURRENT_SOURCE_DIR}/3rd/lib/libunwind-x86_64.a)
endif()

# static library
if (LOG2_STATIC_LIB MATCHES "ON")
    add_library(log2_static STATIC ${SOURCE_FILES})
    add_library(eular::log2_static ALIAS log2_static)

    target_include_directories(log2_static
        PUBLIC
            ${ROOT_PATH}/include
            ${ROOT_PATH}/include/callstack
            ${ROOT_PATH}/include/log2
        PRIVATE
            ${ROOT_PATH}
        INTERFACE
            $<BUILD_INTERFACE:${ROOT_PATH}>
    )

    target_compile_definitions(log2_static PRIVATE LOG_STATIC)

    if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
        target_link_libraries(log2_static pthread)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
        target_compile_options(log2_static PRIVATE /utf-8)
        target_compile_definitions(log2_static PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()
endif()

# dynamic library
if (LOG2_DYNAMIC_LIB MATCHES "ON")
    add_library(log2 SHARED ${SOURCE_FILES})
    add_library(eular::log2 ALIAS log2)
    target_include_directories(log2
        PUBLIC
            ${ROOT_PATH}/include
            ${ROOT_PATH}/include/callstack
            ${ROOT_PATH}/include/log2
        PRIVATE
            ${ROOT_PATH}
        INTERFACE
            $<BUILD_INTERFACE:${ROOT_PATH}>
    )

    target_compile_definitions(log2 PRIVATE LOG_EXPORTS)

    if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
        target_link_libraries(log2 pthread)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
        target_compile_options(log2 PRIVATE /utf-8)
        target_compile_definitions(log2 PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()
endif()

install(TARGETS log2
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(FILES ${LOG_HEADER_FILES} DESTINATION include/log2)
install(FILES ${CALLSTACK_HEADER_FILES} DESTINATION include/log2)

# uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)
endif()
