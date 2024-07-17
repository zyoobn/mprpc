#include "mprpcconfig.h"
#include <cstdlib>
#include <iostream>
#include <string>

// 负责解析加载配置文件
void  MprpcConfig::LoadConfigFile(const char* config_file) {
    FILE *pf = fopen(config_file, "r");
    if (pf == nullptr) {
        std::cout << config_file << " is note exist!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // 1.注释   2.正确的配置项   3。去掉开头的多余的空格
    while (!feof(pf)) {
        char buf[512] = {0};
        fgets(buf, 512, pf);

        // 去掉字符串中多余的空格
        std::string read_buf;
        for (uint32_t i = 0; buf[i] != '\0'; ++i) {
            if (buf[i] != ' ') {
                read_buf += buf[i];
            }
        }
        // int idx = read_buf.find_first_not_of(' ');
        // if (idx != std::string::npos) {
        //     // 说明字符串前面有空格
        //     read_buf = read_buf.substr(idx, read_buf.size() - idx);
        // }
        
        // // 去掉字符串后面多余的空格
        // idx = read_buf.find_last_not_of(' ');
        // if (idx != std::string::npos) {
        //     // 说明字符串后面有空格
        //     read_buf = read_buf.substr(0, idx + 1);
        // }
        
        // 判断#的注释
        if (read_buf[0] == '#' || read_buf.empty()) {
            continue;
        }

        // 解析配置项
        int idx = read_buf.find("=");
        if (idx == std::string::npos) {
            // 配置不合法
            continue;
        }

        std::string key;
        std::string value;
        key = read_buf.substr(0, idx);
        value = read_buf.substr(idx + 1, read_buf.size() - (idx + 1));
        if (value.back() == '\n') {
            value.pop_back();
        }
        m_configMap.insert({key, value});
    }
}

// 查询配置项信息
std::string  MprpcConfig::Load(const std::string& key) {
    auto it = m_configMap.find(key);
    if (it == m_configMap.end()) {
        return "";
    }
    return it->second;
}