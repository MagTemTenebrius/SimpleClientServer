#ifndef UNICODE
#define UNICODE
#endif

#include <winsock2.h>
#include <windows.h>
#include <winuser.h>
#include <ws2tcpip.h>
//#include <comutil.h>
//#include <comutil.h>
#include "util/util.h"
#include "util/client.h"


#pragma comment(lib, "ws2_32.lib") //Winsock Library

HWND hCommandEdit;
int hCommandEditID = 5;
HWND hConsoleEdit;
int hConsoleEditID = 2;
HWND hwndButton;
int hwndButtonID = 3;
//HWND hwndButtonConnect;
//int hwndButtonConnectID = 4;

Client client;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR pCmdLine, INT nCmdShow) {

    // Register the window class.
    const wchar_t CLASS_NAME[] = L"ClientServer";

    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    int width = 700;
    int height = 430;

    HWND hwnd = CreateWindowEx(
            0,                              // Optional window styles.
            CLASS_NAME,                     // Window class
            L"ClientServer",    // Window text
            WS_OVERLAPPEDWINDOW,            // Window style
            // Size and position
            (1920 - width) / 2, (1080 - height) / 2, width, height,
            nullptr,       // Parent window
            nullptr,       // Menu
            hInstance,  // Instance handle
            nullptr        // Additional application data
    );

    if (hwnd == nullptr) {
        return 0;
    }

    hCommandEdit = CreateWindow(TEXT("Edit"), TEXT(""),
                                WS_CHILD | WS_VISIBLE | WS_BORDER,
                                15, 325, 540, 20,
                                hwnd, (HMENU) hCommandEditID, nullptr, nullptr);

    hConsoleEdit = CreateWindow(TEXT("Edit"), TEXT(""),
                                WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_AUTOVSCROLL |
                                ES_READONLY | ES_MULTILINE | ES_AUTOVSCROLL,
                                15, 15, 655, 295,
                                hwnd, (HMENU) hConsoleEditID, nullptr, nullptr);

    hwndButton = CreateWindow(
            TEXT("BUTTON"), TEXT("SEND"),
            WS_TABSTOP | WS_VISIBLE | WS_CHILD,
            570, 325, 100, 20,
            hwnd, (HMENU) hwndButtonID, (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), nullptr);

//    hwndButtonConnect = CreateWindow(
//            TEXT("BUTTON"), TEXT("CONNECT"),
//            WS_TABSTOP | WS_VISIBLE | WS_CHILD,
//            570, 355, 100, 20,
//            hwnd, (HMENU) hwndButtonConnectID, (HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE), nullptr);


    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.
    MSG msg = {};

    client.setOutput(hConsoleEdit, (HMENU) hConsoleEditID);
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    PMYDATA pDataArray = (PMYDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
                                             sizeof(MYDATA));;
    switch (uMsg) {
        case WM_COMMAND:
            WCHAR buf[256];
            client.IsSystem();
            if (LOWORD(wParam) == hwndButtonID) {
                WCHAR buf[128];
                client.createAndConnect();
                pDataArray->socket = client.ConnectSocket;
                pDataArray->hwnd = hConsoleEdit;
//                readData(pDataArray);
                CreateThread(
                        NULL,                   // default security attributes
                        0,                      // use default stack size
                        readData,       // thread function name
                        pDataArray,          // argument to thread function
                        0,                      // use default creation flags
                        nullptr);   // returns the thread identifier
                int cTxtLen = GetWindowTextLength(hCommandEdit);
                GetWindowText(hCommandEdit, buf, cTxtLen + 1);
                client.sendData(buf);
                return 0;
//            } else if (LOWORD(wParam) == hwndButtonConnectID) {
//                client.createAndConnect();
//                pDataArray->socket = client.ConnectSocket;
//                pDataArray->hwnd = hConsoleEdit;
//                CreateThread(
//                        NULL,                   // default security attributes
//                        0,                      // use default stack size
//                        readData,       // thread function name
//                        pDataArray,          // argument to thread function
//                        0,                      // use default creation flags
//                        nullptr);   // returns the thread identifier
//                return 0;
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW + 1));
            EndPaint(hwnd, &ps);
        }
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}