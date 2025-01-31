#include "mprpcapplication.h"
#include "rpcheader.pb.h"
#include "logger.h"
#include <cstdlib>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

/*
send_rpc_str:
header_size + service_name method_name args_size + args
*/
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, 
                          const google::protobuf::Message* request,
                          google::protobuf::Message* response, 
                          google::protobuf::Closure* done) {
    const google::protobuf::ServiceDescriptor* service = method->service();
    std::string service_name = service->name();
    std::string method_name = method->name();

    // 获取参数的序列化字符串长度 args_size
    uint32_t args_size = 0;
    std::string args_str;

    if (request->SerializeToString(&args_str)) {
        args_size = args_str.size();
    } else {
        controller->SetFailed("serialize request error!");
        LOG_ERR("serialize request error! %s:%s%d", __FILE__, __FUNCTION__, __LINE__);
        return;
    }

    // 定义rpc的请求header
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    uint32_t header_size = 0;
    std::string rpc_header_str;
    if (rpcHeader.SerializeToString(&rpc_header_str)) {
        header_size = rpc_header_str.size();
    } else {
        controller->SetFailed("serialize rpc header error!");
        LOG_ERR("serialize rpc header error! %s:%s%d", __FILE__, __FUNCTION__, __LINE__);
        return;
    }

    // 组织待发送的rpc请求的字符串
    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char*)&header_size, 4)); //header_size
    send_rpc_str += rpc_header_str; // rpcheader
    send_rpc_str += args_str; //args

    // 打印调试信息
    std::cout << "===========================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "===========================================" << std::endl;

    // 使用tcp编程，完成rpc方法的远程调用
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        controller->SetFailed("create socket error! error:");
        LOG_ERR("create socket error! error:%d %s:%s%d", errno, __FILE__, __FUNCTION__, __LINE__);
        return;
    }

    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = std::stoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport"));

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        close(clientfd);
        std::string errtxt = "connect error! errno:" + std::to_string(errno);
        controller->SetFailed(errtxt);
        LOG_ERR("connect error! errno:%d %s:%s%d", errno, __FILE__, __FUNCTION__, __LINE__);
        return;
    }
    
    //  发送rpc请求
    if (send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0) == -1) {
        close(clientfd);
        std::string errtxt = "send error! errno:" + std::to_string(errno);
        controller->SetFailed(errtxt);
        LOG_ERR("send error! errno:%d %s:%s%d", errno, __FILE__, __FUNCTION__, __LINE__);
        return;
    }

    // 接受rpc请求的响应值
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if ((recv_size = recv(clientfd, recv_buf, 1024, 0)) == -1) {
        close(clientfd);
        std::string errtxt = "recv error! errno:" + std::to_string(errno);
        controller->SetFailed(errtxt);
        LOG_ERR("recv error! errno:%d %s:%s%d", errno, __FILE__, __FUNCTION__, __LINE__);
        return;
    }

    // std::string response_str(recv_buf);
    // std::string response_str(recv_buf, 0, recv_size);

    if (!response->ParseFromArray(recv_buf, recv_size)) {
        close(clientfd);
        std::string errtxt = "parse error! errno:" + std::string(recv_buf);
        controller->SetFailed(errtxt);
        LOG_ERR("parse error! errno:%d %s:%s%d", errno, __FILE__, __FUNCTION__, __LINE__);
        return;
    }
    close(clientfd);
}