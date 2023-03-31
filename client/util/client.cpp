//
// Created by tenebrius on 06.06.2021.
//

#include <cstdio>
#include "client.h"
#include "diffie.h"
#include "hmac.h"
#include "byte.h"


void Client::close() {
    char buffer[128];
    sprintf(buffer, "Recv error %d\n", WSAGetLastError());
    util::appendConsole(outHWND, buffer);
    closesocket(ConnectSocket);
    WSACleanup();
}


void Client::sendData(WCHAR *command, SOCKET my_sock) {
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
    // key
    unsigned int g = 2;
    unsigned int p = 100;
    int a;
    int A = generateNumber(&a, g, p);
    sprintf(buffer, "Generate a = %d\n", a);
    util::appendConsole(outHWND, buffer);
    char result[128];
    sprintf(result, "%d", A);

    sprintf(buffer, "Send A = %d\n", A);
    util::appendConsole(outHWND, buffer);
    iResult = send(ConnectSocket, result, (int) strlen(result), 0);
    if (iResult == SOCKET_ERROR) {
        sprintf(buffer, "send failed with error: %d\n", WSAGetLastError());
        util::appendConsole(outHWND, buffer);
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }

    int bytes_recv;
    unsigned long B;
    while ((bytes_recv = recv(my_sock, &buf[0], sizeof(buf) - 1, 0)) && bytes_recv != SOCKET_ERROR) {
        B = atoi(buf);
        break;
    }

    sprintf(buffer, "Read B = %d\n", B);
    util::appendConsole(outHWND, buffer);
    unsigned long Bh = generateASh(a, B, p);
//    util::out("SecretKEY Bh = %d\n", Bh);
    sprintf(buffer, "SecretKEY Bh = %d\n", Bh);
    util::appendConsole(outHWND, buffer);

//    const char *key = "1234";
    char key[128];
    memset(key, 0, 128);
    itoa(Bh, key, 10);
    unsigned char bytes[MD5HashSize];
    hmac_md5((unsigned char *)c_command, strlen(c_command), (unsigned char *)key, strlen(key), bytes);

    char* hashResult = btoa(bytes, MD5HashSize);
    sprintf(buffer, "hashResult = %s\n", hashResult);
    util::appendConsole(outHWND, buffer);

    // send hash
    iResult = send(ConnectSocket, hashResult, (int) strlen(hashResult), 0);
    if (iResult == SOCKET_ERROR) {
        sprintf(buffer, "send failed with error: %d\n", WSAGetLastError());
        util::appendConsole(outHWND, buffer);
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }
    // send command
    iResult = send(ConnectSocket, c_command, (int) strlen(c_command), 0);
    if (iResult == SOCKET_ERROR) {
        sprintf(buffer, "send failed with error: %d\n", WSAGetLastError());
        util::appendConsole(outHWND, buffer);
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }

    // read hash
    char hashResultServerSended[128];
    memset(hashResultServerSended, 0, 128);
    while ((bytes_recv = recv(my_sock, &hashResultServerSended[0], 128 - 1, 0)) && bytes_recv != SOCKET_ERROR) {
        sprintf(buffer, "hashResultServerSended: %s\n", hashResultServerSended);
        util::appendConsole(outHWND, buffer);
        break;
    }

    int nsize;
    char buff[1024];
    memset(buff, 0, 1024);
    while ((nsize = recv(ConnectSocket, &buff[0], sizeof(buff) - 1, 0)) != SOCKET_ERROR) {

        hmac_md5((unsigned char *)buff, strlen(buff), (unsigned char *)key, strlen(key), bytes);
        char * hashResultServer = btoa(bytes, MD5HashSize);

        sprintf(buffer, "hashResultServer: %s\n", hashResultServer);
        util::appendConsole(outHWND, buffer);

        if (strcmp(hashResultServerSended, hashResultServer) != 0) {
            sprintf(buffer, "Hash BAD! Returned\n");
            util::appendConsole(outHWND, buffer);
            return;
        }

        // выводим на экран
        sprintf(buffer, "(%d)S=>C:%s\n", nsize, buff);
        if (util::startsWith("d|", buff)) {
            util::downloadFile(buff);
        }
        util::appendConsole(outHWND, buffer);
        break;
    }





    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        sprintf(buffer, "shutdown failed with error: %d\n", WSAGetLastError());
        util::appendConsole(outHWND, buffer);
        closesocket(ConnectSocket);
        WSACleanup();
        return;
    }
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

DWORD WINAPI readData(LPVOID lpParam, Client client, WCHAR *buf) {
    SOCKET ConnectSocket = ((PMYDATA) lpParam)->socket;
    HWND hwnd = ((PMYDATA) lpParam)->hwnd;
    // Шаг 4 - чтение и передача сообщений
    int nsize;
    char buff[1024];
    char buffer[1024];
//    while ((nsize = recv(ConnectSocket, &buff[0], sizeof(buff) - 1, 0)) != SOCKET_ERROR) {
//        if (nsize == 0) continue;
//        // ставим завершающий ноль в конце строки
//        buff[nsize] = 0;
//
//        // выводим на экран
//        sprintf(buffer, "(%d)S=>C:%s\n", nsize, buff);
//        if (util::startsWith("d|", buff)) {
//            util::downloadFile(buff);
//        }
//        util::appendConsole(hwnd, buffer);
//    }
    client.sendData(buf, ConnectSocket);


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