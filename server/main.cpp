/*
	Live Server on port 8888
*/
#include<winsock2.h>
#include "util/util.h"
#include "util/handler.h"
#include "util/server.h"

#pragma comment(lib, "ws2_32.lib") //Winsock Library

int main(int argc, char *argv[]) {
    Server server;
    server.init();

    return 0;
}