//
// Created by tenebrius on 06.06.2021.
//

#ifndef CLIENT_SERVER_CLIENT_H
#define CLIENT_SERVER_CLIENT_H

#include <winsock2.h>
#include <windows.h>
#include <winuser.h>
#include <ws2tcpip.h>
#include "util.h"

#pragma comment(lib, "ws2_32.lib") //Winsock Library

#define PORT 8888
#define SERVERADDR "127.0.0.1"

typedef struct MyData {
    SOCKET socket;
    HWND hwnd;
} MYDATA, *PMYDATA;

class Client {
    struct hostent *he;
    struct in_addr **addr_list;
    WSADATA wsa;
    HWND outHWND;
    HMENU outID;
public:
    SOCKET ConnectSocket = INVALID_SOCKET;

    void setOutput(HWND outHwnd, HMENU outID);

    void close();

    void sendData(WCHAR *command, SOCKET my_sock);

    void createAndConnect();

    BOOL IsSystem();
};

DWORD WINAPI readData(LPVOID lpParam, Client client, WCHAR *buf);


#endif //CLIENT_SERVER_CLIENT_H
