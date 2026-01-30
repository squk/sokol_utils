# sokol_utils

Extension utilities for the [sokol](https://github.com/floooh/sokol) headers.

Currently, this repository contains `sokol_app_utils.h`, which provides additional functionality for `sokol_app.h` that isn't available in the core library.

## Usage

Include `sokol_app_utils.h` *after* the `sokol_app.h` implementation. To create the implementation, define `SOKOL_APP_UTILS_IMPL` in exactly one C or C++ file.

```c
#define SOKOL_IMPL
#include "sokol_app.h"

#define SOKOL_APP_UTILS_IMPL
#include "sokol_app_utils.h"
```

## API

```c
void sapp_set_window_position(int x, int y);
void sapp_set_window_size(int w, int h);
void sapp_get_window_position(int *x, int *y);
void sapp_get_window_size(int *w, int *h);
int sapp_screen_width(void);
int sapp_screen_height(void);
bool sapp_window_focused(void);
int sapp_num_monitors(void);
int sapp_current_monitor(void);
void sapp_set_monitor(int index);
void sapp_set_fullscreen(bool enable);
```

*Note: Most of these functions are only supported on desktop platforms (Windows, macOS, Linux).*
