#include "window_utils.h"

void window_fullscreen(HWND hwnd, int width, int height) {
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style &= ~WS_OVERLAPPEDWINDOW;
    style |= WS_POPUP;

    SetWindowLong(hwnd, GWL_STYLE, style);
    SetWindowPos(hwnd, HWND_TOP, 0, 0, width, height, SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
}

void window_windowed(HWND hwnd, int x, int y, int width, int height) {
    LONG style = GetWindowLong(hwnd, GWL_STYLE);
    style |= WS_OVERLAPPEDWINDOW;
    style &= ~WS_POPUP;

    SetWindowLong(hwnd, GWL_STYLE, style);
    SetWindowPos(hwnd, NULL, x, y, width, height, SWP_FRAMECHANGED | SWP_NOOWNERZORDER);
    ShowWindow(hwnd, SW_SHOWNORMAL);
}
