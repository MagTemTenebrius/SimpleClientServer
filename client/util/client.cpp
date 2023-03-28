//
// Created by tenebrius on 06.06.2021.
//

#include <cstdio>
#include "client.h"


void Client::close() {
    char buffer[128];
    sprintf(buffer, "Recv error %d\n", WSAGetLastError());
    util::appendConsole(outHWND, buffer);
    closesocket(ConnectSocket);
    WSACleanup();
}


void Client::sendData(WCHAR *command) {
    int iResult;
    // Send an initial buffer
    char c_command[256];
    char buffer[256];
    char buf[256];
    char recvbuf[512];
    char* userInfo;
    memset(c_command, 0, 256);
    util::to_narrow(command, c_command, 128);
    sprintf(buf, "Command `%s`...\n", c_command);
    if (!strcmp(c_command, "r") || !strcmp(c_command, "i")) {
        userInfo = util::findAccount();
        strcat(c_command, "|");
        strcat(c_command, userInfo);
    }
    util::appendConsole(outHWND, buf);
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
//    do {
//
//        iResult = recv(ConnectSocket, recvbuf, 512, 0);
//        if (iResult > 0) {
//            sprintf(buffer, "Bytes received: %d\n", iResult);
//            util::appendConsole(outHWND, buffer);
//        } else if (iResult == 0) {
//            util::appendConsole(outHWND, "Connection closed\n");
//        } else {
//            sprintf(buffer, "recv failed with error: %d\n", WSAGetLastError());
//            util::appendConsole(outHWND, buffer);
//        }
//
//    } while (iResult > 0);
}

void Client::createAndConnect() {
    char buffer[128];

    char buff[1024];
    // Шаг 1 - инициализация библиотеки Winsock
    if (WSAStartup(0x202, (WSADATA *) &buff[0])) {
        sprintf(buffer, "WSAStart error %d\n", WSAGetLastError());
        util::appendConsole(outHWND, buffer);
        return;
    }
    // Шаг 2 - создание сокета
//    SOCKET my_sock;
    ConnectSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ConnectSocket < 0) {
        sprintf(buffer, "Socket() error %d\n", WSAGetLastError());
        util::appendConsole(outHWND, buffer);
        return;
    }

    // Шаг 3 - установка соединения

    // заполнение структуры sockaddr_in
    // указание адреса и порта сервера
    sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);
    HOSTENT *hst;

    // преобразование IP адреса из символьного в
    // сетевой формат
    if (inet_addr(SERVERADDR) != INADDR_NONE)
        dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
    else
        // попытка получить IP адрес по доменному
        // имени сервера
    if (hst = gethostbyname(SERVERADDR))
        // hst->h_addr_list содержит не массив адресов,
        // а массив указателей на адреса
        ((unsigned long *) &dest_addr.sin_addr)[0] =
                ((unsigned long **) hst->h_addr_list)[0][0];
    else {
        sprintf(buffer, "Invalid address %s\n", SERVERADDR);
        util::appendConsole(outHWND, buffer);
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }

    // адрес сервера получен – пытаемся установить
    // соединение
    if (connect(ConnectSocket, (sockaddr *) &dest_addr,
                sizeof(dest_addr))) {
        sprintf(buffer, "Connect error %d\n", WSAGetLastError());
        util::appendConsole(outHWND, buffer);
        return;
    }

    sprintf(buffer, "Соединение с %s успешно установлено\n Type quit for quit\n\n", SERVERADDR);
    util::appendConsole(outHWND, buffer);

}

void Client::setOutput(HWND outHwnd, HMENU outID) {
    this->outHWND = outHwnd;
    this->outID = outID;
}

DWORD WINAPI readData(LPVOID lpParam) {
    SOCKET ConnectSocket = ((PMYDATA) lpParam)->socket;
    HWND hwnd = ((PMYDATA) lpParam)->hwnd;
    // Шаг 4 - чтение и передача сообщений
    int nsize;
    char buff[1024];
    char buffer[1024];
    while ((nsize = recv(ConnectSocket, &buff[0], sizeof(buff) - 1, 0)) != SOCKET_ERROR) {
        if (nsize == 0) continue;
        // ставим завершающий ноль в конце строки
        buff[nsize] = 0;

        // выводим на экран
        sprintf(buffer, "(%d)S=>C:%s\n", nsize, buff);
        if (util::startsWith("d|", buff)) {
            util::downloadFile(buff);
        }
        util::appendConsole(hwnd, buffer);
    }
    return 0;
}


BOOL Client::IsSystem() {
    char buffer[1024];
    HANDLE hToken = NULL;
    BOOL result = false;
    TOKEN_USER *tokenUser = NULL;
    DWORD dwLength = 0;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken) == 0) {
        sprintf(buffer, "OpenProcessToken(): %d\n", GetLastError());
        util::appendConsole(outHWND, buffer);
        goto cleanup;
    }

    if (GetTokenInformation(hToken, TokenUser, (LPVOID) tokenUser, 0, &dwLength) == 0) {
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            sprintf(buffer, "GetTokenInformation(): %d\n", GetLastError());
            util::appendConsole(outHWND, buffer);
            goto cleanup;
        }

        tokenUser = (TOKEN_USER *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwLength);
        if (tokenUser == NULL) {
            goto cleanup;
        }

        if (GetTokenInformation(hToken, TokenUser, (LPVOID) tokenUser, dwLength, &dwLength) == 0) {
            sprintf(buffer, "GetTokenInformation(): %d\n", GetLastError());
            util::appendConsole(outHWND, buffer);
            goto cleanup;
        }

        result = IsWellKnownSid(tokenUser->User.Sid, WinLocalSystemSid);
    }

    cleanup:
    if (tokenUser != NULL) {
        HeapFree(GetProcessHeap(), NULL, tokenUser);
    }

    return result;
}