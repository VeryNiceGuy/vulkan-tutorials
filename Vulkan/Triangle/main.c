#include <windows.h>
#include "triangle.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"TriangleClass";

    RegisterClass(&wc);

    HWND hWnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        L"Triangle",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT,
        NULL, NULL, hInst, NULL);

    ShowWindow(hWnd, SW_SHOW);
    initialize(hInst, hWnd);

    MSG msg;
    while (true) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            render();
        }
    }

    deinitialize();
    DestroyWindow(hWnd);

    return 0;
}
