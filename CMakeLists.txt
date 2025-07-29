cmake_minimum_required(VERSION 3.0)

project(log2 VERSION 1.0.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

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

add_library(log2 SHARED ${SOURCE_FILES})
add_library(eular::log2 ALIAS log2)

target_include_directories(log2 PUBLIC
        ${ROOT_PATH}/include
        ${ROOT_PATH}/include/callstack
        ${ROOT_PATH}/include/log2
        INTERFACE $<BUILD_INTERFACE:${ROOT_PATH}> $<INSTALL_INTERFACE:include>)

target_compile_definitions(log2 PRIVATE LOG_EXPORTS NDEBUG _CRT_SECURE_NO_WARNINGS)

if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread")
endif()

if (MSVC)
    target_compile_options(log2 PRIVATE /utf-8)
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
