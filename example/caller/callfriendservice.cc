#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "friend.pb.h"
#include <iostream>

int main(int argc, char** argv) {
    // 整个程序启动以后，想使用mprpc框架来享受rpc服务调用，一定需要先调用框架的初始化函数
    MprpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());
    fixbug::GetFriendsListRequest request;
    request.set_userid(2002);

    // rpc方法的响应
    fixbug::GetFriendsListResponse response;

    // 发起rpc方法的调用 同步
    stub.GetFriendsList(nullptr, &request, &response, nullptr); // RpcChannel -> RpcChannel::callMethod 集中来做所有rpc方法调用的参数序列化和网络发送

    // 一次rpc调用完成，读调用结果
    if (response.result().errcode() == 0) {
        std::cout << "rpc GetFriendsList response success!" << std::endl;
        for (int i = 0; i < response.frineds_size(); ++i) {
            std::cout << "index:" << (i + 1) << " " << "name:" << response.frineds(i) << std::endl;
        }
    } else {
        std::cout << "rpc GetFriendsList response:" << response.result().errmsg() << std::endl;
    }
    
    return 0;
}