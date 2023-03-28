//
// Created by tenebrius on 05.06.2021.
//

#ifndef CLIENT_SERVER_UTIL_H
#define CLIENT_SERVER_UTIL_H

#include <windows.h>



typedef struct {
    char *username;
    char *password;
    void *next;
} User;

namespace util {
    void out(const char *message, ...);

    bool startsWith(const char *pre, const char *str);

    char* listing();

    char* download(char* filename);

    bool identify(char* data, bool reg, User** user);

    void initUsers();
}


#endif //CLIENT_SERVER_UTIL_H
