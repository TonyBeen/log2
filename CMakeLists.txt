cmake_minimum_required(VERSION 3.0)

project(log2 VERSION 1.0.0)

set(ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

# 获取目录下所有源文件
file(GLOB_RECURSE SOURCE_FILES *.cpp)

set(LOG_HEADER_FILES
    def.h
    log.h
)

set(CALLSTACK_HEADER_FILES
    callstack/callstack.h
)

add_subdirectory(example)

add_library(log2 SHARED ${SOURCE_FILES})

target_compile_definitions(log2 PRIVATE NDEBUG)

install(TARGETS log2
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(FILES ${LOG_HEADER_FILES} DESTINATION include/log2)
install(FILES ${CALLSTACK_HEADER_FILES} DESTINATION include/callstack)