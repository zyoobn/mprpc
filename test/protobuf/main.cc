#include "test.pb.h"
#include <iostream>
#include <string>

void test1() {
    // 封装了login请求对象的数据
    fixbug::LoginRequest req;
    req.set_name("zhang san");
    req.set_pwd("123456");

    // 对象数据序列化
    std::string send_str;
    if (req.SerializeToString(&send_str)) {
        std::cout << send_str << std::endl;
    }
    
    // 从send_str反序列化一个login请求对象
    fixbug::LoginRequest reqB;
    if (reqB.ParseFromString(send_str)) {
        std::cout << reqB.name() << std::endl;
        std::cout << reqB.pwd() << std::endl;
    }
}

void test2() {
    fixbug::LoginResponse rsp;
    
    fixbug::ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(1);
    rc->set_errmsg("登陆失败");
}

void test3() {
    fixbug::GetFriendListResponse rsp;
    fixbug::ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);

    fixbug::User *user1 = rsp.add_friend_list();
    user1->set_name("zhang san");
    user1->set_age(20);
    user1->set_sex(fixbug::User::MAN);

    fixbug::User *user2 = rsp.add_friend_list();
    user2->set_name("li si");
    user2->set_age(22);
    user2->set_sex(fixbug::User::MAN);

    std::cout << rsp.friend_list_size() << std::endl;

    for (auto& user : rsp.friend_list()) {
        std::cout << user.name() << ' ' << user.age() << ' ' << user.sex() << '\n';
    }
}

int main() {
    // test1();
    // test2();
    test3();


    return 0;
}