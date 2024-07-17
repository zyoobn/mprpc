#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "user.pb.h"
#include <iostream>

int main(int argc, char** argv) {
    // 整个程序启动以后，想使用mprpc框架来享受rpc服务调用，一定需要先调用框架的初始化函数
    MprpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    

    // rpc方法的响应
    fixbug::LoginResponse response;

    // 发起rpc方法的调用 同步
    stub.Login(nullptr, &request, &response, nullptr); // RpcChannel -> RpcChannel::callMethod 集中来做所有rpc方法调用的参数序列化和网络发送

    // 一次rpc调用完成，读调用结果
    if (response.reslut().errcode() == 0) {
        std::cout << "rpc login response:" << response.sucess() << std::endl;
    } else {
        std::cout << "rpc login response:" << response.reslut().errmsg() << std::endl;
    }

    return 0;
}