# sokol_utils

Extension utilities for the [sokol](https://github.com/floooh/sokol) headers.

Currently, this repository contains:
- `sokol_app_utils.h`: Additional functionality for `sokol_app.h`.
- `sokol_gfx_utils.h`: Additional functionality for `sokol_gfx.h` (readback and Metal-only GPU pass timing support).

## Usage

This project's tests assumes that the [sokol](https://github.com/floooh/sokol) repository is located in the parent directory (`../sokol`).

Include the utility headers *after* the corresponding sokol header implementation. To create the implementation, define the corresponding `_IMPL` macro in exactly one C or C++ file.

### sokol_app_utils.h

```c
#define SOKOL_IMPL
#include "sokol_app.h"

#define SOKOL_APP_UTILS_IMPL
#include "sokol_app_utils.h"
```

### sokol_gfx_utils.h

```c
#define SOKOL_IMPL
#include "sokol_gfx.h"

#define SOKOL_GFX_UTILS_IMPL
#include "sokol_gfx_utils.h"
```

## API

### sokol_app_utils.h

```c
void sapp_set_window_position(int x, int y);
void sapp_set_window_size(int w, int h);
void sapp_get_window_position(int *x, int *y);
void sapp_get_window_size(int *w, int *h);
void sapp_set_mouse_position(float x, float y);
void sapp_get_mouse_position(float *x, float *y);
int sapp_display_width(int index);
int sapp_display_height(int index);
bool sapp_window_focused(void);
int sapp_num_displays(void);
int sapp_current_display(void);
void sapp_set_display(int index);
const char* sapp_display_name(int index);
void sapp_set_fullscreen(bool enable);
void sapp_display_position(int index, int *x, int *y);
void sapp_set_window_resizable(bool resizable);
void sapp_set_window_decorated(bool decorated);
bool sapp_can_move_window(void);
void sapp_set_window_visible(bool visible);
bool sapp_window_visible(void);
void sapp_set_swap_interval(int interval);
int sapp_get_swap_interval(void);
```

*Note: Most of these functions are only supported on desktop platforms (Windows, macOS, Linux).*

Mouse positions are relative to the upper-left corner of the window's content
area and use framebuffer pixels, matching `sapp_event.mouse_x` and
`sapp_event.mouse_y`. The getter returns Sokol's last event position so it has
the same capture behavior as mouse events: dragging may report coordinates
outside the content area, movement without capture stops at leave, and the
absolute position remains frozen while the mouse is locked. A warp is reflected
by the getter when Sokol receives the resulting native mouse event. Setting the
position while the mouse is locked, or on an unsupported platform, is a no-op.

### sokol_gfx_utils.h

```c
void* sg_readback_request(sg_image img);
bool sg_readback_is_ready(void* request);
void sg_readback_wait(void* request);
void sg_readback_copy_data(void* request, void* out_pixels);
void sg_readback_destroy(void* request);

#define SG_GPU_PASS_TIMING_MAX_PASSES (256)
#define SG_GPU_PASS_TIMING_LABEL_SIZE (96)

typedef struct sg_gpu_pass_timing {
    char label[SG_GPU_PASS_TIMING_LABEL_SIZE];
    uint64_t frame_index;
    double milliseconds;
} sg_gpu_pass_timing;

bool sg_gpu_pass_timings_setup(void);
void sg_gpu_pass_timings_shutdown(void);
void sg_gpu_pass_timings_set_enabled(bool enabled);
bool sg_gpu_pass_timings_supported(void);
int sg_gpu_pass_timings_poll(sg_gpu_pass_timing* timings, int max_timings);
```

GPU pass timings are Metal-only for now. Call setup before installing
other trace hooks, enable recording as needed, and poll for the oldest completed
frame without blocking the GPU. Provide space for `SG_GPU_PASS_TIMING_MAX_PASSES
+ 1` results: `Frame` is measured independently from the whole command buffer,
followed by labeled passes measured with stage-boundary counter attachments.
Render timings combine vertex/fragment intervals, excluding gaps and counting
their overlap once. Pass timings can overlap and must not be summed as frame time. Invalid or empty
counter intervals are omitted. Devices without stage counters return only the
`Frame` timing. Call shutdown after `sg_shutdown` has drained in-flight work.
