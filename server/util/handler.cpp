//
// Created by tenebrius on 06.06.2021.
//

#include <cstdio>
#include "handler.h"
#include "util.h"
#include "hmac.h"
#include "byte.h"
#include "diffie.h"

int currentThread = 0;
HANDLE client_thread[] = {0, 0};

DWORD WINAPI handler::handlerClients(LPVOID client_socket) {
    util::out("start check threads..\n");
    currentThread = (currentThread + 1) % (sizeof(client_thread)/sizeof(client_thread[0]));
    while (client_thread[currentThread] != 0) {
        WaitForSingleObject(client_thread[currentThread], INFINITE); // wait until the thread is fired
        client_thread[currentThread] = 0;
        if (client_thread[currentThread] != 0) {
            CloseHandle(client_thread[currentThread]);
            util::out("Close thread %d..\n", currentThread);
        }
    }
    HANDLE hThread = CreateThread(NULL, 0, handlerClient, client_socket, 0, NULL);
    client_thread[currentThread] = hThread;
    util::out("Open thread %d..\n", currentThread);
    return 0;
}

DWORD WINAPI handler::handlerClient(LPVOID client_socket) {
    util::out("handler..\n");
    SOCKET my_sock;
    my_sock = ((SOCKET *) client_socket)[0];
    char buff[20 * 1024];
#define sHELLO "Hello, Sailor\r\n"

    // отправляем клиенту приветствие
    util::out("send..\n");
//    send(my_sock, sHELLO, sizeof(sHELLO), 0);

    // цикл эхо-сервера: прием строки от клиента и
    // возвращение ее клиенту
    util::out("go loop..\n");
    int bytes_recv;
    int A;
    int b;
    unsigned int g = 2;
    unsigned int p = 100;
    // KEY
    while ((bytes_recv = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) && bytes_recv != SOCKET_ERROR) {
        util::out("Read A: %s\n", buff);
        A = atoi(buff);
        break;
    }

    int B = generateNumber(&b, g, p);
    util::out("Generate b = %d\n", b);
    char result[128];
    sprintf(result, "%d", B);

    send(my_sock, result, strlen(result), 0);
    util::out("Send B: %d\n", B);

    unsigned int Ah = generateASh(b, A, p);
    util::out("SecretKEY Ah = %d\n", Ah);

    char hashResultClientSended[128];
    memset(hashResultClientSended, 0, 128);
    while ((bytes_recv = recv(my_sock, &hashResultClientSended[0], 128 - 1, 0)) && bytes_recv != SOCKET_ERROR) {
        util::out("hashResultClientSended: %s\n", hashResultClientSended);
        break;
    }

    char key[128];
    memset(key, 0, 128);
    itoa(Ah, key, 10);
    unsigned char bytes[MD5HashSize];
    memset(bytes, 0, MD5HashSize);
    memset(buff, 0, sizeof(buff));
    while ((bytes_recv = recv(my_sock, &buff[0], sizeof(buff), 0)) && bytes_recv != SOCKET_ERROR) {
        util::out("Read %s\n", buff);
        util::out("Read len %d\n", strlen(buff));
        hmac_md5((unsigned char *)buff, strlen(buff), (unsigned char *)key, strlen(key), bytes);
        char * hashResultClient = btoa(bytes, MD5HashSize);
        util::out("Hash result: %s\n", hashResultClient);
        if (strcmp(hashResultClientSended, hashResultClient) != 0) {
            util::out("Hash BAD! Returned\n");
            return 0;
        }

        char * hashResultServer;

        util::out("Read AFTHER %s\n", buff);
        util::out("Read AFTHER len %d\n", strlen(buff));
        if (util::startsWith("ls", buff)) {
            char* files = util::listing();

            hmac_md5((unsigned char *)files, strlen(files), (unsigned char *)key, strlen(key), bytes);
            hashResultServer = btoa(bytes, MD5HashSize);
            send(my_sock, hashResultServer, strlen(hashResultServer), 0);
            util::out("Server send hash: %s\n", hashResultServer);

            send(my_sock, files, strlen(files), 0);
            free(files);
        } else if (util::startsWith("d ", buff)) {
            char* file_data = util::download(buff + 2);
            if (file_data == NULL){
                hmac_md5((unsigned char *)"Ошибка чтения файла", strlen("Ошибка чтения файла"), (unsigned char *)key, strlen(key), bytes);
                hashResultServer = btoa(bytes, MD5HashSize);
                send(my_sock, hashResultServer, strlen(hashResultServer), 0);
                util::out("Server send hash: %s\n", hashResultServer);

                send(my_sock, "Ошибка чтения файла", sizeof("Ошибка чтения файла"), 0);
            }
            else {
                hmac_md5((unsigned char *)file_data, strlen(file_data), (unsigned char *)key, strlen(key), bytes);
                hashResultServer = btoa(bytes, MD5HashSize);
                send(my_sock, hashResultServer, strlen(hashResultServer), 0);
                util::out("Server send hash: %s\n", hashResultServer);

                send(my_sock, file_data, strlen(file_data), 0);
                free(file_data);
            }
//            send(my_sock, &buff[0], bytes_recv, 0);
        } else if (util::startsWith("i", buff)) {
            User* lastUserIdent;
            bool resultLogin = util::identify(buff, false, &lastUserIdent);
            if (resultLogin) {
                char hello[256];
                int lenHello = sprintf(hello, "Добро пожаловать, %s", lastUserIdent->username);

                hmac_md5((unsigned char *)hello, strlen(hello), (unsigned char *)key, strlen(key), bytes);
                hashResultServer = btoa(bytes, MD5HashSize);
                send(my_sock, hashResultServer, strlen(hashResultServer), 0);
                util::out("Server send hash: %s\n", hashResultServer);

                send(my_sock, hello, lenHello, 0);
            } else {

                hmac_md5((unsigned char *)"Пользователь не найден", strlen("Пользователь не найден"), (unsigned char *)key, strlen(key), bytes);
                hashResultServer = btoa(bytes, MD5HashSize);
                send(my_sock, hashResultServer, strlen(hashResultServer), 0);
                util::out("Server send hash: %s\n", hashResultServer);

                send(my_sock, "Пользователь не найден", sizeof("Пользователь не найден"), 0);
            }
        } else if (util::startsWith("r", buff)) {
            bool resultReg = util::identify(buff, true, 0);
            if (resultReg) {
                hmac_md5((unsigned char *)"Вы успешно зарегались", strlen("Вы успешно зарегались"), (unsigned char *)key, strlen(key), bytes);
                hashResultServer = btoa(bytes, MD5HashSize);
                send(my_sock, hashResultServer, strlen(hashResultServer), 0);
                util::out("Server send hash: %s\n", hashResultServer);

                send(my_sock, "Вы успешно зарегались", sizeof("Вы успешно зарегались"), 0);
            } else {
                hmac_md5((unsigned char *)"Пользователь уже зеган", strlen("Пользователь уже зеган"), (unsigned char *)key, strlen(key), bytes);
                hashResultServer = btoa(bytes, MD5HashSize);
                send(my_sock, hashResultServer, strlen(hashResultServer), 0);
                util::out("Server send hash: %s\n", hashResultServer);

                send(my_sock, "Пользователь уже зеган", sizeof("Пользователь уже зеган"), 0);
            }
        } else {
            hmac_md5((unsigned char *)"command not found", strlen("command not found"), (unsigned char *)key, strlen(key), bytes);
            hashResultServer = btoa(bytes, MD5HashSize);
            send(my_sock, hashResultServer, strlen(hashResultServer), 0);
            util::out("Server send hash: %s\n", hashResultServer);

            send(my_sock, "command not found", sizeof("command not found"), 0);
        }
    }
    int test = WSAGetLastError();

    // если мы здесь, то произошел выход из цикла по
    // причине возращения функцией recv ошибки –
    // соединение клиентом разорвано
//    nclients--; // уменьшаем счетчик активных клиентов
//    printf("-disconnect\n");
//    PRINTNUSERS

    // закрываем сокет
    util::out("disconect\n");
    closesocket(my_sock);
    return 0;
}