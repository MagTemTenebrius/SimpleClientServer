#include "util.h"

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

size_t util::to_narrow(const wchar_t *src, char *dest, size_t dest_len) {
    size_t i;
    wchar_t code;
    i = 0;
    while (src[i] != '\0' && i < (dest_len - 1)) {
        code = src[i];
        if (code < 128)
            dest[i] = char(code);
        else {
            dest[i] = '?';
            if (code >= 0xD800 && code <= 0xD8FF)
                // lead surrogate, skip the next code unit, which is the trail
                i++;
        }
        i++;
    }
    dest[i] = '\0';
    return i - 1;
}


wchar_t* util::charToTchar(const char *src) {
    size_t size = strlen(src) + 1;
    wchar_t *wcstring = new wchar_t[size];
    size_t convertedChars = 0;
    mbstowcs_s(&convertedChars, wcstring, size, src, _TRUNCATE);
    return wcstring;
}


void util::appendConsole(const HWND &hwnd, TCHAR *newText) {
    DWORD StartPos, EndPos;
    SendMessage(hwnd, EM_GETSEL, reinterpret_cast<WPARAM>(&StartPos), reinterpret_cast<WPARAM>(&EndPos));
    int outLength = GetWindowTextLength(hwnd);
    SendMessage(hwnd, EM_SETSEL, outLength, outLength);
    SendMessage(hwnd, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(newText));
    SendMessage(hwnd, EM_SETSEL, StartPos, EndPos);
}