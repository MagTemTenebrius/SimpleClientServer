//
// Created by tenebrius on 06.06.2021.
//

#include "server.h"

void Server::init() {
    util::out("Initialising Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        util::out("Failed. Error Code : %d\n", WSAGetLastError());
        return;
    }

    util::out("Initialised.\n");

    //Create a socket
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        util::out("Could not create socket : %d\n", WSAGetLastError());
    }

    util::out("Socket created.\n");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    //Bind
    if (bind(s, (struct sockaddr *) &server, sizeof(server)) == SOCKET_ERROR) {
        util::out("Bind failed with error code : %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    util::out("Bind done\n");

    //Listen to incoming connections
    listen(s, 3);

    //Accept and incoming connection
    util::out("Waiting for incoming connections...\n");

    int c = sizeof(struct sockaddr_in);

    while ((new_socket = accept(s, (struct sockaddr *) &client, &c)) != INVALID_SOCKET) {
        util::out("Connection accepted. %d.%d.%d.%d:%d\n", client.sin_addr.S_un.S_un_b.s_b1,
                  client.sin_addr.S_un.S_un_b.s_b2, client.sin_addr.S_un.S_un_b.s_b3, client.sin_addr.S_un.S_un_b.s_b4,
                  client.sin_port);

        //Reply to the client
//        char* message = "Hello Client , I have received your connection. But I have to go now, bye\n";
//        send(new_socket, message, strlen(message), 0);
        DWORD thID;
        CreateThread(NULL, NULL, handler::handlerClient,
                     &new_socket, NULL, &thID);
    }

    if (new_socket == INVALID_SOCKET) {
        util::out("accept failed with error code : %d", WSAGetLastError());
        return;
    }

    closesocket(s);
    WSACleanup();
}