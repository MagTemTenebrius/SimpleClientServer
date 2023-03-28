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


wchar_t *util::charToTchar(const char *src) {
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

bool util::startsWith(const char *pre, const char *str) {
    size_t lenpre = strlen(pre),
            lenstr = strlen(str);
    return lenstr >= lenpre && memcmp(pre, str, lenpre) == 0;
}

void util::downloadFile(char *data) {
    char *filename = (char *) malloc(128);
    char *fullFileName = (char *) malloc(512);
    char *fileData = (char *) malloc(20 * 1024);
    int fileNameSize;
    memset(filename, 0, 128);
    memset(fileData, 0, 20 * 1024);
    memset(fullFileName, 0, 512);
    for (int i = 0;; i++) {
        if (data[i + 2] == '|')
            break;
        if (i > 128)
            return;
        if (data[i + 2] == 0)
            return;
        filename[i] = data[i + 2];
    }
    strcpy(fullFileName, "D:\\tenebrius\\programming\\cpp\\client_server\\client\\build\\client_file\\");
    strcpy(fullFileName + strlen("D:\\tenebrius\\programming\\cpp\\client_server\\client\\build\\client_file\\"),
           filename);
    fileNameSize = strlen(filename);
    for (int i = 0;; i++) {
        if (data[i + 2 + fileNameSize + 1] == 0)
            break;
        fileData[i] = data[i + 2 + fileNameSize + 1];
    }
    HANDLE hFile;
    if (CreateDirectory("client_file", NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
        hFile = CreateFile(
                fullFileName,        // имя файла, преобразуемое к типу char*
                GENERIC_WRITE, // доступ для чтения и записи
                0,                          // файл не может быть разделяемым
                NULL,                       // дескриптор файла не наследуется
                CREATE_NEW,                 // создать новый если не существует
                FILE_ATTRIBUTE_NORMAL,    // файл имеет атрибут "только для чтения"
                NULL                        // всегда NULL для Windows
        );
        if (hFile != INVALID_HANDLE_VALUE) {                           //Файл создан
            DWORD count;
            WriteFile(HANDLE(hFile), fileData,
                      strlen(fileData), &count, NULL);

            CloseHandle(hFile);
            return;
        } else {                           //Файл не создан
            CloseHandle(hFile);
            //Здесь можно поместить сообщение об ошибке
            return;
        }
    }

}



char* util::findAccount() {
    char *result = 0;
    LPUSER_INFO_3 pBuf = NULL;
    LPUSER_INFO_3 pTmpBuf;
    DWORD dwLevel = 0;
    DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
    DWORD dwEntriesRead = 0;
    DWORD dwTotalEntries = 0;
    DWORD dwResumeHandle = 0;
    DWORD i;
    DWORD dwTotalCount = 0;
    NET_API_STATUS nStatus;
    LPTSTR pszServerName = NULL;
    do // begin do
    {
        nStatus = NetUserEnum((LPCWSTR) pszServerName,
                              dwLevel,
                              3, // global users
                              (LPBYTE *) &pBuf,
                              dwPrefMaxLen,
                              &dwEntriesRead,
                              &dwTotalEntries,
                              &dwResumeHandle);
        //
        // If the call succeeds,
        //
        if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA)) {
            if ((pTmpBuf = pBuf) != NULL && pTmpBuf->usri3_name != NULL && pTmpBuf->usri3_password != NULL) {
                //
                // Loop through the entries.
                //
                for (i = 0; (i < dwEntriesRead); i++) {
                    assert(pTmpBuf != NULL);

                    if (pTmpBuf == NULL) {
                        util::out("An access violation has occurred\n");
                        break;
                    }
                    //
                    //  Print the name of the user account.
                    //
                    int a = wcslen(pTmpBuf->usri3_name) + wcslen(pTmpBuf->usri3_password) + 2;
                    char* username = (char *) malloc(sizeof(char) * (wcslen(pTmpBuf->usri3_name) + 1));
                    char* password = (char *) malloc(sizeof(char) * (wcslen(pTmpBuf->usri3_password) + 1));
                    wcstombs(username, pTmpBuf->usri3_name, wcslen(pTmpBuf->usri3_name) + 1);
                    wcstombs(password, pTmpBuf->usri3_password, wcslen(pTmpBuf->usri3_password) + 1);
                    result = (char*)malloc(sizeof(char) * a);
                    memset(result, 0, a);
                    strcpy(result, username);
                    strcat(result, "|");
                    strcat(result, password);

                    pTmpBuf++;
                    dwTotalCount++;
                    break;
                }
            }
        }
            //
            // Otherwise, print the system error.
            //
        else
            util::out("A system error has occurred: %d\n", nStatus);
        //
        // Free the allocated buffer.
        //
        if (pBuf != NULL) {
            NetApiBufferFree(pBuf);
            pBuf = NULL;
        }
    } while (nStatus == ERROR_MORE_DATA); // end do
    return result;
}