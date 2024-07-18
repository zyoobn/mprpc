#include "mprpcapplication.h"
#include <cstdlib>
#include <iostream>
#include <unistd.h>

MprpcConfig MprpcApplication::m_config;
bool MprpcApplication::is_init = false;

void ShowArgsHelp() {
    std::cout << "format: command -i <configfile>" << std::endl;
}

bool MprpcApplication::IsInit() {
    return is_init;
}

void MprpcApplication::Init(int argc, char** argv) {
    if (is_init) {
        // std::cout << "Repeat init!" << std::endl;
        return;
    }
    is_init = true;
    if (argc < 2) {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c = 0;
    std::string config_file;
    while ((c = getopt(argc, argv, "i:")) != -1) {
        switch (c) {
            case 'i':
                config_file = optarg;
                break;
            case '?':
                ShowArgsHelp();
                exit(EXIT_FAILURE);
            case ':':
                ShowArgsHelp();
                exit(EXIT_FAILURE);
            default:
                break;
        }
    }

    // 开始加载配置文件了   rpcserver_ip=   rpcserver_port=    zookeeper_ip=   zookepper_port= 
    m_config.LoadConfigFile(config_file.c_str());

    // PrintInitInfo();
}

MprpcApplication& MprpcApplication::GetInstance() {
    static MprpcApplication app;
    return app;
}

MprpcConfig& MprpcApplication::GetConfig() {
    return m_config;
}

void MprpcApplication::PrintInitInfo() {
    std::cout << "rpcserverip:" << m_config.Load("rpcserverip") << std::endl;
    std::cout << "rpcserverport:" << m_config.Load("rpcserverport") << std::endl;
    std::cout << "zookeeperip:" << m_config.Load("zookeeperip") << std::endl;
    std::cout << "zookeeperport:" << m_config.Load("zookeeperport") << std::endl;
}