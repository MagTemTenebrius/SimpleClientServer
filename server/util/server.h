//
// Created by tenebrius on 06.06.2021.
//

#ifndef CLIENT_SERVER_SERVER_H
#define CLIENT_SERVER_SERVER_H

#include <windows.h>
#include "util.h"
#include "handler.h"


class Server {
    WSADATA wsa;
    SOCKET s, new_socket;
    struct sockaddr_in server, client;

public:
    void init();
};


#endif //CLIENT_SERVER_SERVER_H
