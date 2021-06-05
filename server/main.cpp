//
// Created by tenebrius on 05.06.2021.
//
#include <winsock2.h>
#include <winuser.h>
#include "util/util.h"


#pragma comment(lib, "ws2_32.lib") //Winsock Library

int main(int argc, char *argv[]) {
    char buf[256];
    WSADATA wsa;
    SOCKET s;

    util::out("Initialising Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        util::out("Failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }

    util::out("Initialised.\n");


    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        util::out("Could not create socket: %d\n", WSAGetLastError());
    }
    util::out("Socket created.\n");
    return 0;
}