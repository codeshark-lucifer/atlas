#pragma once
#include <windows.h>
#include <engine/utils.h>

typedef HWND Window;

extern Window window;

struct Event {
    MSG msg;
};

bool InitPlatform();
Window CreateWindowPlatform(
    const str& name, 
    const i32& width, 
    const i32& height
);

void PollEvent(Event* event);
bool ShouldClose();
void DestoryPlatform();

void SetTitleBarColor(COLORREF textColor, COLORREF backgroundColor);

void SwapBuffersWindow();