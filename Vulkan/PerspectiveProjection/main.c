#include <windows.h>
#include <windowsx.h>
#include "perspective_projection.h"

int prevMouseX = 0;
int prevMouseY = 0;
int deltaX = 0;
int deltaY = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_MOUSEMOVE: {
        int mouseX = GET_X_LPARAM(lParam);
        int mouseY = GET_Y_LPARAM(lParam);

        deltaX = mouseX - prevMouseX;
        deltaY = mouseY - prevMouseY;

        prevMouseX = mouseX;
        prevMouseY = mouseY;

        mouseMove(deltaX, deltaY);
        break;
    }
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_LEFT:
            moveLeft();
            break;
        case VK_RIGHT:
            moveRight();
            break;
        case VK_UP:
            moveForward();
            break;
        case VK_DOWN:
            moveBackward();
            break;
        default:
            break;
        }
        break;
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
