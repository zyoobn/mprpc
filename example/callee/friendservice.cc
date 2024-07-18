#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include "friend.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"


class FriendService : public fixbug::FriendServiceRpc {
public:
    std::vector<std::string> GetFriendsList(uint32_t userid) {
        std::cout << "do GetFriendList service! userid:" << userid << std::endl;
        std::vector<std::string> vec;
        vec.push_back("zhang san");
        vec.push_back("li si");
        vec.push_back("wang wu");
        return vec;
    }

    // 重写基类方法
    void GetFriendsList(google::protobuf::RpcController* controller,
                       const ::fixbug::GetFriendsListRequest* request,
                       ::fixbug::GetFriendsListResponse* response,
                       ::google::protobuf::Closure* done) {
        uint32_t userid = request->userid();
        
        std::vector<std::string> friends_list = GetFriendsList(userid);

        for (auto& friend_name : friends_list) {
            std::string* name_ptr = response->add_frineds();
            *name_ptr = friend_name;
        }

        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");

        done->Run();
    }
};

int main(int argc, char** argv) {
    // 调用框架的初始化操作   provider -i config.conf
    MprpcApplication::Init(argc, argv);

    // 把UserService对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new FriendService());

    // 启用一个rpc服务发布节点   RUn以后，进程进入阻塞状态，等待远程的rpc调用请求
    provider.Run();

    return 0;
}