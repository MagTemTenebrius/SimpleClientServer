//
// Created by tenebrius on 05.06.2021.
//
#include <winsock2.h>
#include <winuser.h>
#include "console/console.cpp"

#pragma comment(lib, "ws2_32.lib") //Winsock Library

int main(int argc, char *argv[]) {
    char buf[256];
    WSADATA wsa;
    SOCKET s;

    util::out("Initialising Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
//        wsprintf(buf, "Failed. Error Code : %d\n", WSAGetLastError());
        util::out("Failed. Error Code : %d\n", WSAGetLastError());
        return 1;
    }

    util::out("Initialised. %d", 123456);


    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
//        printf("Could not create socket : %d", WSAGetLastError());
    }

//    printf("Socket created.\n");
    return 0;
}