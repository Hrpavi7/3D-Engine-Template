#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct AppWindow AppWindow;

typedef struct AppInput {
    bool keys[256];
    bool quit_requested;
    int mouse_dx;
    int mouse_dy;
    int width;
    int height;
    bool has_focus;
} AppInput;

typedef struct AppWindowDesc {
    const char* title;
    int width;
    int height;
} AppWindowDesc;

bool app_window_create(AppWindow** out_window, AppWindowDesc desc);
void app_window_destroy(AppWindow* window);
void app_window_poll(AppWindow* window, AppInput* io_input);
void app_window_swap_buffers(AppWindow* window);
bool app_window_make_gl_current(AppWindow* window);
void app_window_set_cursor_locked(AppWindow* window, bool locked);

double app_time_seconds(void);

