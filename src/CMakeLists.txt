# aux_source_directory(. SRC_LIST)

set(SRC_LIST mprpcapplication.cc mprpcconfig.cc rpcheader.pb.cc rpcprovider.cc)

# # mprpc动态库
# add_library(mprpc SHARED ${SRC_LIST})

# mprpc静态库
add_library(mprpc ${SRC_LIST})

target_link_libraries(mprpc protobuf muduo_net muduo_base)