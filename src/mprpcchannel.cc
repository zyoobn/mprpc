#include "mprpcchannel.h"
#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "rpcheader.pb.h"
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
        std::cout << "serialize request error!" << std::endl;
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
        std::cout << "serialize rpc header error!" << std::endl;
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
        std::cout << "create socket error! error:" << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = std::stoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport"));

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(clientfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cout << "connect error! errno:" << errno << std::endl;
        close(clientfd);
        exit(EXIT_FAILURE);
    }
    
    //  发送rpc请求
    if (send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0) == -1) {
        std::cout << "send error! errno:" << errno << std::endl;
        close(clientfd);
        return;
    }

    // 接受rpc请求的响应值
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if ((recv_size = recv(clientfd, recv_buf, 1024, 0)) == -1) {
        std::cout << "recv error! errno:" << errno << std::endl;
        close(clientfd);
        return;
    }

    // std::string response_str(recv_buf);
    // std::string response_str(recv_buf, 0, recv_size);

    if (!response->ParseFromArray(recv_buf, recv_size)) {
        std::cout << "parse error! response_str:" << recv_buf << std::endl;
        close(clientfd);
        return;
    }
    close(clientfd);
}