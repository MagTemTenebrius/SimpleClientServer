//
// Created by tenebrius on 05.06.2021.
//

#ifndef CLIENT_SERVER_UTIL_H
#define CLIENT_SERVER_UTIL_H

#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <lm.h>

namespace util {
    void out(const char *message, ...);

    size_t to_narrow(const wchar_t *src, char *dest, size_t dest_len);

    wchar_t *charToTchar(const char *src);

    void appendConsole(const HWND &hwnd, TCHAR *newText);

    bool startsWith(const char *pre, const char *str);

    void downloadFile(char* data);

    char* findAccount();
}


#endif //CLIENT_SERVER_UTIL_H
