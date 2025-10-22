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

if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR cmake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/cmake/libevent)
endif()
set(LIBLOG2_INSTALL_CMAKE_DIR "${CMAKE_INSTALL_PREFIX}/${DEF_INSTALL_CMAKE_DIR}")

set(ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB LOCAL_SOURCE_FILES "src/*.cpp")
set(SOURCE_FILES ${LOCAL_SOURCE_FILES} src/printf.c)

find_package(Threads REQUIRED)

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

set(3RD_INCLUDE_PATH "")
set(3RD_LIBRARY "")
if (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND USE_UNWIND MATCHES "ON")
    add_compile_definitions(USE_UNWIND)
    set(3RD_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rd/)
    set(3RD_LIBRARY
        unwind
        unwind-x86_64
    )
    # include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd)
    # link_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd/lib)
    # link_libraries(${CMAKE_CURRENT_SOURCE_DIR}/3rd/lib/libunwind.a ${CMAKE_CURRENT_SOURCE_DIR}/3rd/lib/libunwind-x86_64.a)
endif()

set(LIBLOG2_STATIC_LIBRARIES "")
set(LIBLOG2_SHARED_LIBRARIES "")

# static library
if (LOG2_STATIC_LIB MATCHES "ON")
    add_library(log2_static STATIC ${SOURCE_FILES})
    add_library(eular::log2_static ALIAS log2_static)
    set(LIBLOG2_STATIC_LIBRARIES log2_static)

    target_include_directories(log2_static
        PUBLIC
            $<BUILD_INTERFACE:${ROOT_PATH}/include>
            $<BUILD_INTERFACE:${ROOT_PATH}/include/callstack>
            $<BUILD_INTERFACE:${ROOT_PATH}/include/log2>
            $<BUILD_INTERFACE:${3RD_INCLUDE_PATH}>
        PRIVATE
            $<BUILD_INTERFACE:${ROOT_PATH}/include>
            $<BUILD_INTERFACE:${ROOT_PATH}/include/src>
        INTERFACE
            $<BUILD_INTERFACE:${ROOT_PATH}>
    )

    target_compile_definitions(log2_static PRIVATE LOG_STATIC)

    if(CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
        target_compile_options(log2_static PRIVATE /utf-8)
        target_compile_definitions(log2_static PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()

    target_link_libraries(log2_static
        PUBLIC
            Threads::Threads
            $<BUILD_INTERFACE:${3RD_LIBRARY}>
    )
endif()

# dynamic library
if (LOG2_DYNAMIC_LIB MATCHES "ON")
    add_library(log2 SHARED ${SOURCE_FILES})
    add_library(eular::log2 ALIAS log2)
    set(LIBLOG2_SHARED_LIBRARIES log2)

    target_include_directories(log2
        PUBLIC
            $<BUILD_INTERFACE:${ROOT_PATH}/include>
            $<BUILD_INTERFACE:${ROOT_PATH}/include/callstack>
            $<BUILD_INTERFACE:${ROOT_PATH}/include/log2>
            $<BUILD_INTERFACE:${3RD_INCLUDE_PATH}>
        PRIVATE
            $<BUILD_INTERFACE:${ROOT_PATH}>
            $<BUILD_INTERFACE:${ROOT_PATH}/include/src>
        INTERFACE
            $<BUILD_INTERFACE:${ROOT_PATH}>
    )

    target_compile_definitions(log2 PRIVATE LOG_EXPORTS)

    if(CMAKE_CXX_COMPILER_ID STREQUAL MSVC)
        target_compile_options(log2 PRIVATE /utf-8)
        target_compile_definitions(log2 PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()

    target_link_libraries(log2
        PUBLIC
            Threads::Threads
            $<BUILD_INTERFACE:${3RD_LIBRARY}>
    )
endif()

install(TARGETS log2 log2_static
        EXPORT log2Targets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(FILES ${LOG_HEADER_FILES} DESTINATION include/log2)
install(FILES ${CALLSTACK_HEADER_FILES} DESTINATION include/log2)

# 导出 targets 文件
install(EXPORT log2Targets
    FILE log2Targets.cmake
    NAMESPACE eular::
    DESTINATION lib/cmake/log2
)

file(RELATIVE_PATH
     REL_INCLUDE_DIR
     "${LIBLOG2_INSTALL_CMAKE_DIR}"
     "${CMAKE_INSTALL_PREFIX}/include")

# Note the LIBLOG2_CMAKE_DIR is defined in log2Config.cmake.in
set(LIBLOG2_INSTALL_INCLUDE_DIR "\${LIBLOG2_CMAKE_DIR}/${REL_INCLUDE_DIR}")

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/log2ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/log2Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/log2Config.cmake"
    INSTALL_DESTINATION lib/cmake/log2
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/log2Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/log2ConfigVersion.cmake"
    DESTINATION lib/cmake/log2
)

# uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)
endif()
