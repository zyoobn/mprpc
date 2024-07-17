# 设置源文件列表
set(SRC_LIST userservice.cc ../user.pb.cc)

# 创建一个名为 provider 的可执行文件
add_executable(provider ${SRC_LIST})

# 将可执行文件 provider 链接到 mprpc 和 protobuf 库
target_link_libraries(provider mprpc)
