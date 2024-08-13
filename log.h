/*************************************************************************
    > File Name: log.h
    > Author: hsz
    > Brief:
    > Created Time: 2024年08月07日 星期三 13时08分33秒
 ************************************************************************/

#ifndef __EULAR_LOG2_H__
#define __EULAR_LOG2_H__

#include "def.h"
#include <stdio.h>

typedef enum {
    LEVEL_DEBUG     = 0,
    LEVEL_INFO      = 1,
    LEVEL_WARN      = 2,
    LEVEL_ERROR     = 3,
    LEVEL_FATAL     = 4,
} log_level_t;

typedef enum {
    OUTPUT_STD      = 0,
    OUTPUT_FILE     = 1,
    OUTPUT_CONSOLE  = 2,
    OUTPUT_SIZE
} output_t;

#ifndef LOGD
#define LOGD(...) (LogWrite(LEVEL_DEBUG, LOG_TAG, __VA_ARGS__))
#endif

#ifndef LOGI
#define LOGI(...) (LogWrite(LEVEL_INFO, LOG_TAG, __VA_ARGS__))
#endif

#ifndef LOGW
#define LOGW(...) (LogWrite(LEVEL_WARN, LOG_TAG, __VA_ARGS__))
#endif

#ifndef LOGE
#define LOGE(...) (LogWrite(LEVEL_ERROR, LOG_TAG, __VA_ARGS__))
#endif

#ifndef LOGF
#define LOGF(...) (LogWrite(LEVEL_FATAL, LOG_TAG, __VA_ARGS__))
#endif

#ifndef LOG_ASSERT
#define LOG_ASSERT(cond, ...) \
    (!(cond) ? ((void)LogWriteAssert(LEVEL_FATAL, #cond, LOG_TAG, __VA_ARGS__)) : (void)0)
#endif

#ifndef LOG_ASSERT2
#define LOG_ASSERT2(cond)   \
    (!(cond) ? ((void)LogWriteAssert(LEVEL_FATAL, #cond, LOG_TAG, nullptr)) : (void)0)
#endif

EXTERN_C_BEGIN

/**
 * @brief 设置最小日志输出级别 (Thread safety)
 *
 * @param level 
 */
LOG_EXPORT void LogSetLevel(log_level_t level = LEVEL_DEBUG);

/**
 * @brief 设置日志输出目录(UTF-8), 默认可执行程序目录 (Thread not safety)
 *
 * @param logFilePath 日志路径
 */
LOG_EXPORT void LogSetLogPath(const char *logFilePath);

/**
 * @brief 添加一个输出类型 (Thread not safety)
 *
 * @param outputType 
 */
LOG_EXPORT void LogOutputAppend(output_t outputType);

/**
 * @brief 删除一个输出类型 (Thread not safety)
 *
 * @param outputType 
 */
LOG_EXPORT void LogOutputDel(output_t outputType);

/**
 * @brief 设置格式化格式 (Thread not safety)
 *
 * @param formatPattern 
 */
LOG_EXPORT void LogSetFormatPattern(const char *formatPattern);

/**
 * @brief 格式化字符串(printf) (Thread safety)
 *
 */
LOG_EXPORT void LogWrite(log_level_t level, const char *log_tag, const char *fmt, ...) ATTR_FORMAT(3, 4);

/**
 * @brief 格式化字符串(printf) (Thread safety)
 *
 */
LOG_EXPORT void LogWriteAssert(log_level_t level, const char *expr, const char *log_tag, const char *fmt, ...) ATTR_FORMAT(4, 5);

EXTERN_C_END

#endif // __EULAR_LOG2_H__
