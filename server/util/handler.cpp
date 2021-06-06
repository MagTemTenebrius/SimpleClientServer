//
// Created by tenebrius on 06.06.2021.
//

#include "handler.h"
#include "util.h"

DWORD WINAPI handler::handlerClient(LPVOID client_socket) {
    SOCKET my_sock;
    my_sock = ((SOCKET *) client_socket)[0];
    char buff[20 * 1024];
#define sHELLO "Hello, Sailor\r\n"

    // отправляем клиенту приветствие
    send(my_sock, sHELLO, sizeof(sHELLO), 0);

    // цикл эхо-сервера: прием строки от клиента и
    // возвращение ее клиенту
    int bytes_recv;
    while ((bytes_recv = recv(my_sock, &buff[0], sizeof(buff), 0)) && bytes_recv != SOCKET_ERROR)
        send(my_sock, &buff[0], bytes_recv, 0);

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