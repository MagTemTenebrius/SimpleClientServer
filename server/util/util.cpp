#include "util.h"



User *users;
void util::initUsers() {
    users = (User *) malloc(sizeof(User));
    users->username = 0;
    users->password = 0;
    users->next = 0;
}

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

bool util::startsWith(const char *pre, const char *str) {
    size_t lenpre = strlen(pre),
            lenstr = strlen(str);
    return lenstr >= lenpre && memcmp(pre, str, lenpre) == 0;
}

char *util::listing() {
    WIN32_FIND_DATA data;
    char *string_data = (char *) malloc(20 * 1024);
    memset(string_data, 0, 20 * 1024);
    strcpy(string_data, "Файлы: ");
    if (CreateDirectory("server_file", NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
        HANDLE hFind = FindFirstFile(".\\server_file\\*", &data);  // FILES

        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (data.cFileName[0] == '.')
                    continue;
                if (string_data[0] != 0)
                    strcat(string_data, ", ");
                strcat(string_data, data.cFileName);
            } while (FindNextFile(hFind, &data));
            FindClose(hFind);
        }
    } else {
        HANDLE stdOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (stdOut != nullptr && stdOut != INVALID_HANDLE_VALUE) {
            DWORD written = 0;
            WriteConsole(stdOut, "Cant create dir", sizeof("Cant create dir"), &written, nullptr);
        }
    }
    return string_data;
}

char *util::download(char *filename) {
    HFILE fileHandle;
    OFSTRUCT tOfStr;
    int sizeFileName =
            strlen(filename) + strlen("D:\\tenebrius\\programming\\cpp\\client_server\\server\\build\\server_file\\");
    char *full_name = (char *) malloc(sizeFileName + 5);
    memset(full_name, 0, sizeFileName);
    memcpy(full_name, "D:\\tenebrius\\programming\\cpp\\client_server\\server\\build\\server_file\\",
           strlen("D:\\tenebrius\\programming\\cpp\\client_server\\server\\build\\server_file\\"));
    strcat(full_name, filename);
    fileHandle = OpenFile(full_name, &tOfStr, OF_READ);
    if (fileHandle == HFILE_ERROR) {
        CloseHandle(HANDLE(fileHandle));
        //Здесь может быть помещено сообщение об ошибке
        return NULL;
    }
    char *file_data = (char *) malloc(20 * 1024);
    memset(file_data, 0, 20 * 1024);
    strcpy(file_data, "d|");
    strcpy(file_data + 2, filename);
    strcpy(file_data + 2 + strlen(filename), "|");
    DWORD count_read;
    if (strstr(filename, ".."))
        return NULL;
    // Делается попытка произвести синхронную операцию чтения.
    WINBOOL bResult = ReadFile(HANDLE(fileHandle), file_data + 2 + strlen(filename) + 1,
                               20 * 1024 - strlen(filename) - 3,
                               &count_read, NULL);
    if (!bResult) {
        DWORD dwError = GetLastError();
        if (!dwError)
            return NULL;
    }
    return file_data;
}


bool util::identify(char *data, bool reg, User** user) {
    char *param1 = (char *) malloc(128);
    char *param2 = (char *) malloc(128);
    int param1Size;
    int param2Size;
    memset(param1, 0, 128);
    memset(param2, 0, 128);
    for (int i = 0;; i++) {
        if (data[i + 2] == '|')
            break;
        if (i > 128)
            break;
        if (data[i + 2] == 0)
            break;
        param1[i] = data[i + 2];
    }
    param1Size = strlen(param1);
    strcpy(param2, &data[2 + param1Size + 1]);
    param2Size = strlen(param2);
    if (reg) {
        User *lasUser = users;
        for (; lasUser->next != 0; lasUser = (User *) lasUser->next) {
            if (lasUser->username != 0 && lasUser->password != 0 && strcmp(lasUser->username, param1) == 0 &&
                strcmp(lasUser->password, param2) == 0)
                return false;
        }
        User *new_user = (User *) malloc(sizeof(User));
        new_user->username = param1;
        new_user->password = param2;
        new_user->next = 0;
        lasUser->next = new_user;
    } else {
        User *lasUser = users;
        for (; lasUser->next != 0; lasUser = (User *) lasUser->next) {
            if (lasUser->username != 0 && lasUser->password != 0 && strcmp(lasUser->username, param1) == 0 &&
                strcmp(lasUser->password, param2) == 0) {
                *user = lasUser;
                return true;
            }
        }
        return false;
    }

}