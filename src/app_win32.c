#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "app.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct AppWindow {
    HWND hwnd;
    HDC hdc;
    HGLRC glrc;
    bool cursor_locked;
    bool raw_input_registered;
    int last_width;
    int last_height;
};

static LARGE_INTEGER g_qpc_freq;
static bool g_qpc_inited;

static void qpc_init_once(void) {
    if (g_qpc_inited) return;
    QueryPerformanceFrequency(&g_qpc_freq);
    g_qpc_inited = true;
}

double app_time_seconds(void) {
    qpc_init_once();
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    return (double)t.QuadPart / (double)g_qpc_freq.QuadPart;
}

static LRESULT CALLBACK app_wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    AppWindow* w = (AppWindow*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);
    switch (msg) {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:
            if (w) {
                w->last_width = (int)LOWORD(lParam);
                w->last_height = (int)HIWORD(lParam);
            }
            return 0;
        default:
            return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
}

static bool register_raw_mouse(HWND hwnd) {
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01;
    rid.usUsage = 0x02;
    rid.dwFlags = RIDEV_INPUTSINK;
    rid.hwndTarget = hwnd;
    return RegisterRawInputDevices(&rid, 1, sizeof(rid)) != FALSE;
}

static bool set_pixel_format(HDC hdc) {
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pf = ChoosePixelFormat(hdc, &pfd);
    if (pf == 0) return false;
    if (!SetPixelFormat(hdc, pf, &pfd)) return false;
    return true;
}

typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hdc, HGLRC shareContext, const int* attribList);
static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB_;

static void* wgl_get_proc(const char* name) {
    void* p = (void*)wglGetProcAddress(name);
    if (p) return p;
    HMODULE opengl32 = GetModuleHandleA("opengl32.dll");
    if (!opengl32) return NULL;
    return (void*)GetProcAddress(opengl32, name);
}

static void load_wgl_extensions(void) {
    wglCreateContextAttribsARB_ = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wgl_get_proc("wglCreateContextAttribsARB");
}

static HGLRC create_best_context(HDC hdc, HGLRC share) {
    if (!wglCreateContextAttribsARB_) {
        return wglCreateContext(hdc);
    }

    const int attribs[] = {
        0x2091, 3,
        0x2092, 3,
        0x9126, 0x00000001,
        0
    };
    HGLRC rc = wglCreateContextAttribsARB_(hdc, share, attribs);
    if (rc) return rc;
    return wglCreateContext(hdc);
}

bool app_window_create(AppWindow** out_window, AppWindowDesc desc) {
    if (!out_window) return false;
    *out_window = NULL;

    WNDCLASSEXA wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = app_wndproc;
    wc.hInstance = GetModuleHandleA(NULL);
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.lpszClassName = "VoxelMcWindowClass";
    if (!RegisterClassExA(&wc)) return false;

    DWORD style = WS_OVERLAPPEDWINDOW;
    RECT r = { 0, 0, desc.width, desc.height };
    AdjustWindowRect(&r, style, FALSE);

    HWND hwnd = CreateWindowExA(
        0,
        wc.lpszClassName,
        desc.title ? desc.title : "OpenGL Test",
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        r.right - r.left,
        r.bottom - r.top,
        NULL,
        NULL,
        wc.hInstance,
        NULL
    );
    if (!hwnd) return false;

    HDC hdc = GetDC(hwnd);
    if (!hdc) {
        DestroyWindow(hwnd);
        return false;
    }

    if (!set_pixel_format(hdc)) {
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
        return false;
    }

    HGLRC temp = wglCreateContext(hdc);
    if (!temp) {
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
        return false;
    }

    if (!wglMakeCurrent(hdc, temp)) {
        wglDeleteContext(temp);
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
        return false;
    }

    load_wgl_extensions();

    HGLRC glrc = create_best_context(hdc, NULL);
    if (!glrc) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(temp);
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
        return false;
    }

    if (glrc != temp) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(temp);
        if (!wglMakeCurrent(hdc, glrc)) {
            wglDeleteContext(glrc);
            ReleaseDC(hwnd, hdc);
            DestroyWindow(hwnd);
            return false;
        }
    }

    AppWindow* w = (AppWindow*)calloc(1, sizeof(AppWindow));
    if (!w) {
        wglMakeCurrent(NULL, NULL);
        wglDeleteContext(glrc);
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
        return false;
    }

    w->hwnd = hwnd;
    w->hdc = hdc;
    w->glrc = glrc;
    w->cursor_locked = false;
    w->raw_input_registered = register_raw_mouse(hwnd);
    w->last_width = desc.width;
    w->last_height = desc.height;

    SetWindowLongPtrA(hwnd, GWLP_USERDATA, (LONG_PTR)w);
    ShowWindow(hwnd, SW_SHOW);
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    *out_window = w;
    return true;
}

void app_window_destroy(AppWindow* window) {
    if (!window) return;
    app_window_set_cursor_locked(window, false);
    wglMakeCurrent(NULL, NULL);
    if (window->glrc) wglDeleteContext(window->glrc);
    if (window->hwnd && window->hdc) ReleaseDC(window->hwnd, window->hdc);
    if (window->hwnd) DestroyWindow(window->hwnd);
    free(window);
}

bool app_window_make_gl_current(AppWindow* window) {
    if (!window) return false;
    return wglMakeCurrent(window->hdc, window->glrc) == TRUE;
}

void app_window_swap_buffers(AppWindow* window) {
    if (!window) return;
    SwapBuffers(window->hdc);
}

static void lock_cursor_to_client(HWND hwnd, bool locked) {
    if (!locked) {
        ClipCursor(NULL);
        ShowCursor(TRUE);
        return;
    }

    RECT rc;
    GetClientRect(hwnd, &rc);
    POINT p0 = { rc.left, rc.top };
    POINT p1 = { rc.right, rc.bottom };
    ClientToScreen(hwnd, &p0);
    ClientToScreen(hwnd, &p1);

    RECT clip = { p0.x, p0.y, p1.x, p1.y };
    ClipCursor(&clip);
    ShowCursor(FALSE);
}

void app_window_set_cursor_locked(AppWindow* window, bool locked) {
    if (!window) return;
    window->cursor_locked = locked;
    lock_cursor_to_client(window->hwnd, locked);
}

static void poll_raw_input(AppInput* io_input, MSG* msg) {
    if (msg->message != WM_INPUT) return;
    UINT size = 0;
    GetRawInputData((HRAWINPUT)msg->lParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
    if (size == 0) return;

    uint8_t stack_buf[256];
    uint8_t* data = stack_buf;
    if (size > sizeof(stack_buf)) {
        data = (uint8_t*)malloc(size);
        if (!data) return;
    }

    if (GetRawInputData((HRAWINPUT)msg->lParam, RID_INPUT, data, &size, sizeof(RAWINPUTHEADER)) == size) {
        RAWINPUT* ri = (RAWINPUT*)data;
        if (ri->header.dwType == RIM_TYPEMOUSE) {
            io_input->mouse_dx += (int)ri->data.mouse.lLastX;
            io_input->mouse_dy += (int)ri->data.mouse.lLastY;
        }
    }

    if (data != stack_buf) free(data);
}

void app_window_poll(AppWindow* window, AppInput* io_input) {
    if (!window || !io_input) return;

    io_input->mouse_dx = 0;
    io_input->mouse_dy = 0;

    MSG msg;
    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            io_input->quit_requested = true;
            break;
        }

        if (msg.message == WM_INPUT) {
            poll_raw_input(io_input, &msg);
        }

        if (msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN) {
            if (msg.wParam < 256) io_input->keys[msg.wParam] = true;
        }
        if (msg.message == WM_KEYUP || msg.message == WM_SYSKEYUP) {
            if (msg.wParam < 256) io_input->keys[msg.wParam] = false;
        }

        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    RECT rc;
    GetClientRect(window->hwnd, &rc);
    io_input->width = rc.right - rc.left;
    io_input->height = rc.bottom - rc.top;
    io_input->has_focus = (GetForegroundWindow() == window->hwnd);
}
