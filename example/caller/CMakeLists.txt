# 设置源文件列表
# set(SRC_LIST calluserservice.cc ../user.pb.cc)
set(SRC_LIST callfriendservice.cc ../friend.pb.cc)

# 创建一个名为 provider 的可执行文件
add_executable(consumer ${SRC_LIST})

# 将可执行文件 provider 链接到 mprpc 和 protobuf 库
target_link_libraries(consumer mprpc)
