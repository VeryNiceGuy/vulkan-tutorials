#include <windows.h>
#include <windowsx.h>
#include "perspective_projection.h"
#include "obj_loader.h"

bool fullscreen = false;
int prevMouseX = 0;
int prevMouseY = 0;
int deltaX = 0;
int deltaY = 0;

/*
void switchToFullscreen(HWND hwnd, int width, int height) {
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~WS_OVERLAPPEDWINDOW;
    style |= WS_POPUP;

    SetWindowLong(hwnd, GWL_STYLE, style);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, width, height, SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
}*/

void restoreWindowed(HWND hwnd) {
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style |= WS_OVERLAPPEDWINDOW;
    style &= ~WS_POPUP;

    SetWindowLong(hwnd, GWL_STYLE, style);
    SetWindowPos(hwnd, NULL, 100, 100, 800, 600, SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
    ShowWindow(hwnd, SW_SHOWNORMAL);
}

void RegisterRawInput(HWND hwnd) {
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01;
    rid.usUsage = 0x02;
    rid.dwFlags = RIDEV_INPUTSINK;
    rid.hwndTarget = hwnd;

    RegisterRawInputDevices(&rid, 1, sizeof(rid));
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_INPUT: {
        BYTE lpb[128];
        PUINT pcbSize;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &pcbSize, sizeof(RAWINPUTHEADER));

        RAWINPUT* raw = (RAWINPUT*)lpb;
        if (raw->header.dwType == RIM_TYPEMOUSE) {
            int deltaX = raw->data.mouse.lLastX;
            int deltaY = raw->data.mouse.lLastY;

            mouseMove(deltaX, deltaY);
        }
        return 0;
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
        case VK_SPACE:
            enableFullScreen();
            if (!fullscreen) {
                switchToFullscreen(hWnd, 1920, 1080);
                fullscreen = true;
            }
            else {
                restoreWindowed(hWnd);
                fullscreen = false;
            }
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

    RegisterRawInput(hWnd);

    ShowWindow(hWnd, SW_SHOW);
    initialize(hInst, hWnd);
    ObjModel model;
    load_obj_model("Gear2.obj", &model);

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
