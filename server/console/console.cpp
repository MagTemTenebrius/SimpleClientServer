//
// Created by tenebrius on 05.06.2021.
//

#include "console.h"

void util::out(const char *message, ...) {
    char buf[256];
    va_list argptr;
    int len;
    va_start(argptr, message);
    len = wvsprintf(buf, message, argptr);
    va_end(argptr);
    HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (stdOut != nullptr && stdOut != INVALID_HANDLE_VALUE) {
        DWORD written = 0;
        WriteConsole(stdOut, buf, len, &written, nullptr);
    }
}