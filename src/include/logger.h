#ifndef LOGGER_H
#define LOGGER_H

#include "lockqueue.h"
#include <string>

enum LogLevel {
    INFO,  // 普通信息
    ERROR, // 错误信息
};

// Mprpc框架提供的日志系统
class Logger {
public:
    // 设置日志级别
    void SetLogLevel(LogLevel level);

    // 写日志
    void Log(const std::string& msg);

    static Logger& GetInstance();

private:
    int m_loglevel; // 记录日志级别
    LockQueue<std::string> m_lockQue;

    Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;
};

// 定义宏  LOG_INFO("XXX %d %s", 20, "xxx")
#define LOG_INFO(logmsgformat, ...) \
    do { \
        Logger& logger = Logger::GetInstance(); \
        logger.SetLogLevel(INFO); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c); \
    } while (false)
#define LOG_ERR(logmsgformat, ...) \
    do { \
        Logger &logger = Logger::GetInstance(); \
        logger.SetLogLevel(ERROR); \
        char c[1024] = {0}; \
        snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c); \
    } while (false)
#endif