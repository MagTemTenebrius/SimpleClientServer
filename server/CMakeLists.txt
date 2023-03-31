cmake_minimum_required(VERSION 3.19)
project(server)

set(CMAKE_CXX_STANDARD 20)

add_executable(server util/util.h util/util.cpp util/handler.h util/handler.cpp util/server.h util/server.cpp util/byte.h util/byte.cpp util/hmac.h util/hmac.cpp util/md5.h util/md5.cpp
        util/diffie.h util/diffie.cpp main.cpp)

target_link_libraries(server wsock32 ws2_32)