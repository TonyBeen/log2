cmake_minimum_required(VERSION 3.0)

project(log2 VERSION 1.0.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB LOCAL_SOURCE_FILES "*.cpp")
file(GLOB_RECURSE CALLSTACK_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/callstack/*.cpp")
set(SOURCE_FILES ${LOCAL_SOURCE_FILES} ${CALLSTACK_SOURCE_FILES})

set(LOG_HEADER_FILES
    def.h
    log.h
)

set(CALLSTACK_HEADER_FILES
    callstack/callstack.h
)

add_subdirectory(example)

add_library(log2 SHARED ${SOURCE_FILES})
add_library(eular::log2 ALIAS log2)

target_include_directories(log2 PRIVATE ${ROOT_PATH}
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
install(FILES ${CALLSTACK_HEADER_FILES} DESTINATION include/callstack)