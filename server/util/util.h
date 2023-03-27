//
// Created by tenebrius on 05.06.2021.
//

#ifndef CLIENT_SERVER_UTIL_H
#define CLIENT_SERVER_UTIL_H

#include <windows.h>

namespace util {
    void out(const char *message, ...);

    bool startsWith(const char *pre, const char *str);

    char* listing();

    char* download(char* filename);
}


#endif //CLIENT_SERVER_UTIL_H
