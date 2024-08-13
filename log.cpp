/*************************************************************************
    > File Name: log.cpp
    > Author: hsz
    > Brief:
    > Created Time: 2024年08月07日 星期三 17时40分34秒
 ************************************************************************/

#include "log.h"

#include <assert.h>
#include <stdarg.h>

#include <mutex>
#include <atomic>
#include <memory>
#include <list>

#include "callstack/callstack.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#define FILE_SIZE       5 * 1024 * 1024
#define FILE_NUMBER     3

#define LOG_TAG_SIZE    (64)
#define MSG_BUF_SIZE    (2048)

static thread_local char g_logBuffer[MSG_BUF_SIZE] = {0};

struct LogPrivate {
    std::string                                     logFilePath{"./"}; // 末尾携带 '/'
    std::string                                     formatPattern;
    std::atomic<int32_t>                            level{log_level_t::LEVEL_DEBUG};
    std::vector<std::shared_ptr<spdlog::logger>>    logger_list;

    LogPrivate() {
        // 1900-1-1 00:00:00.000 3500 3501 [W] log_tag: log message
        formatPattern = "%Y-%m-%d %H:%M:%S.%e %P %t [%L]: %v";
        logger_list.resize(output_t::OUTPUT_SIZE, nullptr);
        logger_list[output_t::OUTPUT_STD] = spdlog::stdout_color_mt("console");
        logger_list[output_t::OUTPUT_STD]->set_level(spdlog::level::level_enum::debug);
    }
};

std::once_flag g_logPrivateInitFlag;
static std::unique_ptr<LogPrivate> g_logPrivate{nullptr};

std::string GetFileName()
{
    time_t curr = time(nullptr);
    struct tm *p = localtime(&curr);

    char buf[128] = { 0 };
    snprintf(buf, sizeof(buf), "log_%.4d%.2d%.2d.log", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday);

    return buf;
}

LogPrivate *GetLogPrivate()
{
    std::call_once(g_logPrivateInitFlag, [] () {
        if (g_logPrivate == nullptr) {
            g_logPrivate.reset(new (std::nothrow) LogPrivate());
        }

        assert(g_logPrivate != nullptr);
    });

    return g_logPrivate.get();
}

void Log2Sink(log_level_t level, const char *msg)
{
    LogPrivate *pLogPrivate = GetLogPrivate();
    for (const auto &it : pLogPrivate->logger_list) {
        if (it == nullptr) {
            continue;
        }

        switch (level) {
        case log_level_t::LEVEL_DEBUG:
            it->debug("{}", msg);
            break;
        case log_level_t::LEVEL_INFO:
            it->info("{}", msg);
            break;
        case log_level_t::LEVEL_WARN:
            it->warn("{}", msg);
            break;
        case log_level_t::LEVEL_ERROR:
            it->error("{}", msg);
            break;
        case log_level_t::LEVEL_FATAL:
            it->critical("{}", msg);
            break;

        default:
            break;
        }
    }
}

void LogSetLevel(log_level_t level)
{
    if (log_level_t::LEVEL_DEBUG <= level && level <= log_level_t::LEVEL_FATAL) {
        GetLogPrivate()->level.store(level, std::memory_order_release);
    }
}

void LogOutputAppend(output_t outputType)
{
    LogPrivate *pLogPrivate = GetLogPrivate();
    switch (outputType) {
    case output_t::OUTPUT_STD:
        if (pLogPrivate->logger_list[output_t::OUTPUT_STD] == nullptr) {
            auto spLogger = spdlog::stdout_color_mt("console");
            spLogger->set_pattern(pLogPrivate->formatPattern);
            spLogger->set_level(spdlog::level::level_enum::debug);
            pLogPrivate->logger_list[output_t::OUTPUT_STD] = spLogger;
        }
        break;
    case output_t::OUTPUT_FILE:
        if (pLogPrivate->logger_list[output_t::OUTPUT_FILE] == nullptr) {
            std::string filePath = pLogPrivate->logFilePath + GetFileName();
            auto spLogger = spdlog::rotating_logger_mt("root", filePath, FILE_SIZE, FILE_NUMBER);
            spLogger->set_pattern(pLogPrivate->formatPattern);
            spLogger->set_level(spdlog::level::level_enum::debug);
            pLogPrivate->logger_list[output_t::OUTPUT_FILE] = spLogger;
        }
    default:
        break;
    }
}

void LogOutputDel(output_t outputType)
{
    LogPrivate *pLogPrivate = GetLogPrivate();
    switch (outputType) {
    case output_t::OUTPUT_STD:
        if (pLogPrivate->logger_list[output_t::OUTPUT_STD] != nullptr) {
            pLogPrivate->logger_list[output_t::OUTPUT_STD] = nullptr;
        }
        break;
    case output_t::OUTPUT_FILE:
        if (pLogPrivate->logger_list[output_t::OUTPUT_FILE] != nullptr) {
            pLogPrivate->logger_list[output_t::OUTPUT_FILE] = nullptr;
        }
    default:
        break;
    }
}

void LogSetFormatPattern(const char *formatPattern)
{
    LogPrivate *pLogPrivate = GetLogPrivate();
    pLogPrivate->formatPattern = formatPattern;
    for (const auto &it : pLogPrivate->logger_list) {
        if (it != nullptr) {
            it->set_pattern(formatPattern);
        }
    }
}

void LogSetLogPath(const char *logFilePath)
{
    std::string logPath = logFilePath;
    std::replace_if(logPath.begin(), logPath.end(), [](char c) -> bool {
        return c == '\\';
    }, '/');

    std::string::iterator begin =
        std::unique(logPath.begin(), logPath.end(), [](char a, char b) -> bool {
            return a == '/' && b == '/';
        });

    logPath.erase(begin, logPath.end());
    if (logPath.back() != '/') {
        logPath.push_back('/');
    }

    GetLogPrivate()->logFilePath = logPath;
}

void LogWrite(log_level_t level, const char *log_tag, const char *fmt, ...)
{
    LogPrivate *pLogPrivate = GetLogPrivate();
    if (pLogPrivate->level.load(std::memory_order_acquire) > level) {
        return;
    }

    if (log_tag == nullptr) {
        log_tag = "Unknow";
    }

    if (fmt == nullptr) {
        return;
    }

    int32_t offset = 0;
    int32_t tagSize = strlen(log_tag);
    tagSize = std::min(tagSize, LOG_TAG_SIZE);
    strncpy(g_logBuffer, log_tag, tagSize);
    offset += tagSize;

    strcpy(g_logBuffer + offset, ": ");
    offset += 2;

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(g_logBuffer + offset, MSG_BUF_SIZE - offset - 1, fmt, ap);
    va_end(ap);

    Log2Sink(level, g_logBuffer);
}

void LogWriteAssert(log_level_t level, const char *expr, const char *log_tag, const char *fmt, ...)
{
    LogPrivate *pLogPrivate = GetLogPrivate();
    if (pLogPrivate->level.load(std::memory_order_acquire) > level) {
        return;
    }

    if (log_tag == nullptr) {
        log_tag = "Unknow";
    }

    size_t offset = snprintf(g_logBuffer, MSG_BUF_SIZE - 1, "assertion \"%s\" failed. ", expr);
    if (fmt != nullptr) {
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(g_logBuffer + offset, MSG_BUF_SIZE - offset - 1, fmt, ap);
        va_end(ap);
    }

    Log2Sink(level, g_logBuffer);

    utils::CallStack stack;
    stack.update();
    const auto &stackFrames = stack.frames();
    for (const auto &it : stackFrames) {
        Log2Sink(level, it.c_str());
    }
}