//
// Created by tenebrius on 06.06.2021.
//

#include <cwchar>
#include <cstdio>
#include "client.h"


void Client::close() {
    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();
}


void Client::sendData(WCHAR *command) {
    int iResult;
    // Send an initial buffer
    char c_command[128];
    char buffer[256];
    util::to_narrow(command, c_command, 128);
    char recvbuf[512];

    int cTxtLen = GetWindowTextLength(outHWND);
    GetWindowText(outHWND, buffer,
                  cTxtLen + 1);
    sprintf(buffer, "Command `%s`...\n", buffer);
    util::appendConsole(outHWND, buffer);
    iResult = send(ConnectSocket, c_command, (int) strlen(c_command), 0);
    if (iResult == SOCKET_ERROR) {
        sprintf(buffer, "send failed with error: %d\n", WSAGetLastError());
        util::appendConsole(outHWND, buffer);
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }

    sprintf(buffer, "Bytes Sent: %ld\n", iResult);
    util::appendConsole(outHWND, buffer);

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        sprintf(buffer, "shutdown failed with error: %d\n", WSAGetLastError());
        util::appendConsole(outHWND, buffer);
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }

    // Receive until the peer closes the connection
    do {

        iResult = recv(ConnectSocket, recvbuf, 512, 0);
        if (iResult > 0) {
            sprintf(buffer, "Bytes received: %d\n", iResult);
            util::appendConsole(outHWND, buffer);
        } else if (iResult == 0) {
            util::appendConsole(outHWND, "Connection closed\n");
        } else {
            sprintf(buffer, "recv failed with error: %d\n", WSAGetLastError());
            util::appendConsole(outHWND, buffer);
        }

    } while (iResult > 0);
}

void Client::createAndConnect() {
    char ip[100];
    int i;
    char *hostname = "localhost";
    char buffer[128];

    sprintf(buffer, "%s...\n", hostname);
    util::appendConsole(outHWND, buffer);

    util::appendConsole(outHWND, "Initialising Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        sprintf(buffer, "Failed. Error Code: %d\n", WSAGetLastError());
        util::appendConsole(outHWND, buffer);
        return;
    }

    util::appendConsole(outHWND, "Initialised.\n");


    if ((he = gethostbyname(hostname)) == NULL) {
        sprintf(buffer, "gethostbyname failed : %d\n", WSAGetLastError());
        util::appendConsole(outHWND, buffer);
        return;
    }

    //Cast the h_addr_list to in_addr , since h_addr_list also has the ip address in long format only
    addr_list = (struct in_addr **) he->h_addr_list;

    for (i = 0; addr_list[i] != NULL; i++) {
        //Return the first one;
        strcpy(ip, inet_ntoa(*addr_list[i]));
    }

    sprintf(buffer, "%s resolved to : %s\n", hostname, ip);
    util::appendConsole(outHWND, buffer);

    struct addrinfo *result = NULL,
            *ptr = NULL,
            hints;
    int iResult;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(ip, "8888", &hints, &result);
    if (iResult != 0) {
        sprintf(buffer, "getaddrinfo failed with error: %d\n", iResult);
        util::appendConsole(outHWND, buffer);
        WSACleanup();
        return;
    }

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                               ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            sprintf(buffer, "socket failed with error: %ld\n", WSAGetLastError());
            util::appendConsole(outHWND, buffer);
            WSACleanup();
            return;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int) ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }


//    freeaddrinfo(result);
//
//    if (ConnectSocket == INVALID_SOCKET) {
//        util::appendConsole(outHWND, "Unable to connect to server!\n");
//        WSACleanup();
//        return;
//    }

}

void Client::setOutput(HWND outHwnd, HMENU outID) {
    this->outHWND = outHwnd;
    this->outID = outID;
}

DWORD WINAPI readData(LPVOID lpParam) {
    SOCKET ConnectSocket = ((PMYDATA)lpParam)->socket;
    // Шаг 4 - чтение и передача сообщений
    int nsize;
    char buff[1024];
    while ((nsize = recv(ConnectSocket, &buff[0], sizeof(buff) - 1, 0)) != SOCKET_ERROR) {
        // ставим завершающий ноль в конце строки
        buff[nsize] = 0;

        // выводим на экран
        util::out("S=>C:%s", buff);

        // читаем пользовательский ввод с клавиатуры
//        util::out("S<=C:");
//        fgets(&buff[0],sizeof(buff)-1, stdin);

        // проверка на "quit"
        if (!strcmp(&buff[0], "quit\n")) {
            // Корректный выход
            util::out("Exit...");
            closesocket(ConnectSocket);
            WSACleanup();
            return 0;
        }
        // передаем строку клиента серверу
        send(ConnectSocket, &buff[0], nsize, 0);
    }
    return 0;
}