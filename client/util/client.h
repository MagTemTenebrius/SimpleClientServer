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

typedef struct MyData {
    SOCKET socket;
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

    void sendData(WCHAR *command);

    void createAndConnect();
};

DWORD WINAPI readData(LPVOID lpParam);


#endif //CLIENT_SERVER_CLIENT_H
