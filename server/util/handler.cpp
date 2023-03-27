//
// Created by tenebrius on 06.06.2021.
//

#include "handler.h"
#include "util.h"

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
    while ((bytes_recv = recv(my_sock, &buff[0], sizeof(buff), 0)) && bytes_recv != SOCKET_ERROR) {
        util::out("loop\n");
        if (util::startsWith("ls", buff)) {
            char* files = util::listing();
            send(my_sock, files, strlen(files), 0);
            free(files);
        } else if (util::startsWith("d ", buff)) {
            char* file_data = util::download(buff + 2);
            if (file_data == NULL)
                send(my_sock, "Ошибка чтения файла", sizeof("Ошибка чтения файла"), 0);
            else {
                send(my_sock, file_data, strlen(file_data), 0);
                free(file_data);
            }
//            send(my_sock, &buff[0], bytes_recv, 0);
        } else {
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