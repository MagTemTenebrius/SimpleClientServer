cmake_minimum_required(VERSION 3.19)
project(client)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS} -static")
#link_libraries(ws2_32 wsock32)

add_executable(client WIN32 util/util.h util/util.cpp util/client.h util/client.cpp main.cpp util/md5.cpp util/md5.h util/hmac.cpp util/hmac.h util/byte.cpp util/byte.h util/diffie.cpp util/diffie.h)

target_link_libraries(client wsock32 ws2_32 netapi32)