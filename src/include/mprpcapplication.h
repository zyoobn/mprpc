#ifndef MPRPCAPPLICATION_H
#define MPRPCAPPLICATION_H

#include "mprpcconfig.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"

// mprpc框架的初始化类
class MprpcApplication {
public:
    static void Init(int argc, char** argv);
    static MprpcApplication& GetInstance();
    static MprpcConfig& GetConfig();
    static bool IsInit();
    static void PrintInitInfo();
private:
    static MprpcConfig m_config;
    static bool is_init;
    MprpcApplication() {}
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&) = delete;
    MprpcApplication& operator=(const MprpcApplication&) = delete;
    MprpcApplication& operator=(MprpcApplication&&) = delete;
};
#endif