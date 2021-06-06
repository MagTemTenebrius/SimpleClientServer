//
// Created by tenebrius on 06.06.2021.
//

#ifndef CLIENT_SERVER_HANDLER_H
#define CLIENT_SERVER_HANDLER_H

#include <windows.h>

namespace handler {
    DWORD WINAPI handlerClient(LPVOID client_socket);
};


#endif //CLIENT_SERVER_HANDLER_H
