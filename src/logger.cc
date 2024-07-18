#include "logger.h"
#include <bits/types/FILE.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <time.h>

Logger& Logger::GetInstance() {
    static Logger logger;
    return logger;
}

Logger::Logger() {
    std::thread writeLogTask([this]() {
        while (true) {
            // 获取当前的日期，然后取日志信息，写入相应的日志文件当中
            time_t now = time(nullptr);
            tm* nowtm = localtime(&now);

            std::string file_name;
            file_name = std::to_string(nowtm->tm_yday + 1900) + "-"
                        + std::to_string(nowtm->tm_mon + 1) + "-"
                        + std::to_string(nowtm->tm_mday) + "-"
                        + "log.txt"; 

            FILE* pf = fopen(file_name.c_str(), "a+");
            if (pf == nullptr) {
                std::cout << "logger file : " << file_name << " open error!" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg = m_lockQue.Pop();

            std::string time_buf;
            time_buf = std::to_string(nowtm->tm_hour) + ":"
                        + std::to_string(nowtm->tm_min) + ":"
                        + std::to_string(nowtm->tm_sec) + " => ";
            msg = time_buf + "[" + (m_loglevel == INFO ? "info" : "error") + "]" + msg;
            fputs(msg.c_str(), pf);
            fclose(pf);
        }
    });

    // 设置分离线程，守护线程
    writeLogTask.detach();
}

// 设置日志级别
void Logger::SetLogLevel(LogLevel level) {
    m_loglevel = level;
}

// 写日志，把日志信息写入lockqueue缓冲区当中
void Logger::Log(const std::string& msg) {
    m_lockQue.Push(msg + "\n");
}
