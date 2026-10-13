#if defined(SOKOL_IMPL) && !defined(SOKOL_APP_UTILS_IMPL)
#define SOKOL_APP_UTILS_IMPL
#endif
#ifndef SOKOL_APP_UTILS_INCLUDED
#define SOKOL_APP_UTILS_INCLUDED

/*
    sokol_app_utils.h -- extension utilities for sokol_app.h

    Project URL: https://github.com/floooh/sokol

    Include this header *after* the sokol_app.h implementation to access
    internal state.

    Do this:
        #define SOKOL_IMPL or
        #define SOKOL_APP_UTILS_IMPL
    before you include this file in *one* C or C++ file to create the
    implementation.
*/

#if !defined(SOKOL_APP_INCLUDED)
#error "Please include sokol_app.h before sokol_app_utils.h"
#endif

#if defined(SOKOL_API_DECL) && !defined(SOKOL_APP_UTILS_API_DECL)
#define SOKOL_APP_UTILS_API_DECL SOKOL_API_DECL
#endif
#ifndef SOKOL_APP_UTILS_API_DECL
#define SOKOL_APP_UTILS_API_DECL extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* set the window position (only on desktop platforms) */
SOKOL_APP_UTILS_API_DECL void sapp_set_window_position(int x, int y);
/* set the window size (content area) (only on desktop platforms) */
SOKOL_APP_UTILS_API_DECL void sapp_set_window_size(int w, int h);
/* get the window position (only on desktop platforms) */
SOKOL_APP_UTILS_API_DECL void sapp_get_window_position(int *x, int *y);
/* get the window size (content area) (only on desktop platforms) */
SOKOL_APP_UTILS_API_DECL void sapp_get_window_size(int *w, int *h);
/* set the mouse position in framebuffer pixels relative to the window's
   content area (only on desktop platforms) */
SOKOL_APP_UTILS_API_DECL void sapp_set_mouse_position(float x, float y);
/* get the last mouse position reported by Sokol's event pipeline in
   framebuffer pixels relative to the window's content area */
SOKOL_APP_UTILS_API_DECL void sapp_get_mouse_position(float *x, float *y);
/* returns the width of a display by index in pixels */
SOKOL_APP_UTILS_API_DECL int sapp_display_width(int index);
/* returns the height of a display by index in pixels */
SOKOL_APP_UTILS_API_DECL int sapp_display_height(int index);
/* returns true if the window has focus */
SOKOL_APP_UTILS_API_DECL bool sapp_window_focused(void);
/* returns the number of connected displays (only on desktop platforms) */
SOKOL_APP_UTILS_API_DECL int sapp_num_displays(void);
/* get the current display index (only on desktop platforms) */
SOKOL_APP_UTILS_API_DECL int sapp_current_display(void);
/* set the current display (only on desktop platforms) */
SOKOL_APP_UTILS_API_DECL void sapp_set_display(int index);
/* get the display name by index (only on desktop platforms) */
SOKOL_APP_UTILS_API_DECL const char* sapp_display_name(int index);
/* sets fullscreen mode (wrapper around platform specific logic) */
SOKOL_APP_UTILS_API_DECL void sapp_set_fullscreen(bool enable);
/* set the swap interval:
    0: VSync off (unthrottled rendering)
    1: VSync on (standard refresh rate, e.g. 60 FPS)
    2+: Adaptive/Lower refresh (e.g. 2 = 30 FPS on 60Hz display)
*/
SOKOL_APP_UTILS_API_DECL void sapp_set_swap_interval(int interval);
/* get the current swap interval */
SOKOL_APP_UTILS_API_DECL int sapp_get_swap_interval(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SOKOL_APP_UTILS_INCLUDED */

/*-- IMPLEMENTATION ----------------------------------------------------------*/
#ifdef SOKOL_APP_UTILS_IMPL
#define SOKOL_APP_UTILS_IMPL_INCLUDED (1)

#ifndef SOKOL_API_IMPL
#define SOKOL_API_IMPL
#endif

/* Forward declarations of internal sokol_app.h functions */
#if defined(SOKOL_VULKAN)
_SOKOL_PRIVATE void _sapp_vk_recreate_swapchain(void);
#endif

#if defined(_SAPP_MACOS)
_SOKOL_PRIVATE void _sapp_macos_set_window_position(int x, int y) {
  if (_sapp.macos.window) {
    NSArray *screens = [NSScreen screens];
    if (screens && [screens count] > 0) {
      NSScreen *primary = [screens objectAtIndex:0];
      NSRect primary_frame = [primary frame];
      NSRect window_frame = [_sapp.macos.window frame];
      CGFloat new_x = (CGFloat)x;
      CGFloat new_y =
          primary_frame.size.height - (CGFloat)y - window_frame.size.height;
      [_sapp.macos.window setFrameOrigin:NSMakePoint(new_x, new_y)];
    }
  }
}

_SOKOL_PRIVATE void _sapp_macos_set_window_size(int w, int h) {
  if (_sapp.macos.window) {
    NSSize size = NSMakeSize((CGFloat)w, (CGFloat)h);
    [_sapp.macos.window setContentSize:size];
  }
}

_SOKOL_PRIVATE void _sapp_macos_get_window_position(int *x, int *y) {
  if (_sapp.macos.window) {
    NSRect window_frame = [_sapp.macos.window frame];
    if (x)
      *x = (int)window_frame.origin.x;
    if (y) {
      NSArray *screens = [NSScreen screens];
      if (screens && [screens count] > 0) {
        NSScreen *primary = [screens objectAtIndex:0];
        NSRect primary_frame = [primary frame];
        *y = (int)(primary_frame.size.height - window_frame.origin.y -
                   window_frame.size.height);
      } else {
        *y = 0;
      }
    }
  } else {
    if (x)
      *x = 0;
    if (y)
      *y = 0;
  }
}

_SOKOL_PRIVATE void _sapp_macos_get_window_size(int *w, int *h) {
  if (_sapp.macos.window) {
    NSRect content_rect =
        [_sapp.macos.window contentRectForFrameRect:[_sapp.macos.window frame]];
    if (w)
      *w = (int)content_rect.size.width;
    if (h)
      *h = (int)content_rect.size.height;
  } else {
    if (w)
      *w = 0;
    if (h)
      *h = 0;
  }
}

_SOKOL_PRIVATE void _sapp_macos_set_mouse_position(float x, float y) {
  if (_sapp.macos.window) {
    const CGFloat scale = (CGFloat)_sapp.dpi_scale;
    const NSPoint window_position = NSMakePoint(
        (CGFloat)x / scale,
        ((CGFloat)_sapp.framebuffer_height - (CGFloat)y - 1.0) / scale);
    const NSPoint screen_position =
        [_sapp.macos.window convertPointToScreen:window_position];
    NSArray *screens = [NSScreen screens];
    if (screens && [screens count] > 0) {
      const NSRect primary_frame = [[screens objectAtIndex:0] frame];
      CGWarpMouseCursorPosition(CGPointMake(
          screen_position.x, NSMaxY(primary_frame) - screen_position.y));
    }
  }
}

_SOKOL_PRIVATE bool _sapp_macos_window_focused(void) {
  if (_sapp.macos.window) {
    return [_sapp.macos.window isKeyWindow];
  }
  return false;
}

_SOKOL_PRIVATE int _sapp_macos_display_width(int index) {
  NSArray *screens = [NSScreen screens];
  if (screens && index >= 0 && (NSUInteger)index < [screens count]) {
    NSScreen *screen = [screens objectAtIndex:(NSUInteger)index];
    return (int)[screen frame].size.width;
  }
  return 0;
}

_SOKOL_PRIVATE int _sapp_macos_display_height(int index) {
  NSArray *screens = [NSScreen screens];
  if (screens && index >= 0 && (NSUInteger)index < [screens count]) {
    NSScreen *screen = [screens objectAtIndex:(NSUInteger)index];
    return (int)[screen frame].size.height;
  }
  return 0;
}

_SOKOL_PRIVATE int _sapp_macos_num_displays(void) {
  NSArray *screens = [NSScreen screens];
  return (int)[screens count];
}

_SOKOL_PRIVATE int _sapp_macos_current_display(void) {
  if (_sapp.macos.window) {
    NSScreen *screen = [_sapp.macos.window screen];
    NSArray *screens = [NSScreen screens];
    NSUInteger index = [screens indexOfObject:screen];
    if (index != NSNotFound) {
      return (int)index;
    }
  }
  return 0;
}

_SOKOL_PRIVATE void _sapp_macos_set_display(int index) {
  if (_sapp.macos.window) {
    NSArray *screens = [NSScreen screens];
    if ((index >= 0) && (index < (int)[screens count])) {
      /* NOTE: native macOS fullscreen spaces are screen-locked.
         To move a fullscreen window to another screen, we must exit and re-enter.
      */
      bool was_fullscreen = _sapp.fullscreen;
      if (was_fullscreen) {
        _sapp_macos_toggle_fullscreen();
      }
      NSScreen *screen = [screens objectAtIndex:(NSUInteger)index];
      NSRect screen_frame = [screen frame];
      NSRect window_frame = [_sapp.macos.window frame];
      CGFloat x = screen_frame.origin.x +
                  (screen_frame.size.width - window_frame.size.width) / 2.0f;
      CGFloat y = screen_frame.origin.y +
                  (screen_frame.size.height - window_frame.size.height) / 2.0f;
      [_sapp.macos.window setFrameOrigin:NSMakePoint(x, y)];
      if (was_fullscreen) {
        _sapp_macos_toggle_fullscreen();
      }
    }
  }
}

_SOKOL_PRIVATE const char* _sapp_macos_display_name(int index) {
  NSArray *screens = [NSScreen screens];
  if (index >= 0 && index < (int)[screens count]) {
    NSScreen *screen = [screens objectAtIndex:(NSUInteger)index];
    static char buf[256];
    NSString* name = nil;
    if ([screen respondsToSelector:@selector(localizedName)]) {
      name = [screen localizedName];
    }
    if (name) {
      [name getCString:buf maxLength:sizeof(buf) encoding:NSUTF8StringEncoding];
    } else {
      snprintf(buf, sizeof(buf), "Display %d", index);
    }
    return buf;
  }
  return "";
}

_SOKOL_PRIVATE void _sapp_macos_set_swap_interval(int interval) {
    _sapp.desc.swap_interval = interval;
    #if defined(SOKOL_METAL)
    static dispatch_source_t _sapp_macos_unthrottled_timer = nil;
    if (_sapp.mtl.layer) {
        if ([_sapp.mtl.layer respondsToSelector:@selector(setDisplaySyncEnabled:)]) {
            [_sapp.mtl.layer setDisplaySyncEnabled:(interval > 0)];
        }
    }
    if (_sapp_macos_unthrottled_timer) {
        dispatch_source_cancel(_sapp_macos_unthrottled_timer);
        _sapp_macos_unthrottled_timer = nil;
    }
    if (interval == 0) {
        if (_sapp.mtl.display_link) {
            [_sapp.mtl.display_link setPaused:YES];
        }
        _sapp_macos_unthrottled_timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_main_queue());
        dispatch_source_set_timer(_sapp_macos_unthrottled_timer, DISPATCH_TIME_NOW, 100 * NSEC_PER_USEC, 100 * NSEC_PER_USEC);
        dispatch_source_set_event_handler(_sapp_macos_unthrottled_timer, ^{
            [_sapp.macos.view displayLinkFired:nil];
        });
        dispatch_resume(_sapp_macos_unthrottled_timer);
    } else {
        // Keep the timer frame driver active for obscured windows and direct
        // launches. Their paused display link must not supply frame timing.
        if (_sapp.mtl.display_link && nil == _sapp.mtl.obscured_timer) {
            [_sapp.mtl.display_link setPaused:NO];
            if ([_sapp.mtl.display_link respondsToSelector:@selector(setPreferredFrameRateRange:)]) {
                NSInteger max_fps = 60;
                if (_sapp.macos.window) {
                    max_fps = [[_sapp.macos.window screen] maximumFramesPerSecond];
                } else {
                    max_fps = [[NSScreen mainScreen] maximumFramesPerSecond];
                }
                const float preferred_fps = (float)max_fps / (float)interval;
                CAFrameRateRange frame_rate_range = { preferred_fps, preferred_fps, preferred_fps };
                _sapp.mtl.display_link.preferredFrameRateRange = frame_rate_range;
            }
        }
    }
    #endif
    #if defined(SOKOL_GLCORE)
    if (_sapp.macos.view) {
        NSOpenGLContext* ctx = [_sapp.macos.view openGLContext];
        if (ctx) {
            GLint swap_int = (GLint)interval;
            [ctx setValues:&swap_int forParameter:NSOpenGLContextParameterSwapInterval];
        }
    }
    #endif
}
#endif /* _SAPP_MACOS */

#if defined(_SAPP_IOS)
_SOKOL_PRIVATE void _sapp_ios_set_swap_interval(int interval) {
    _sapp.desc.swap_interval = interval;
    #if defined(SOKOL_METAL)
    static dispatch_source_t _sapp_ios_unthrottled_timer = nil;
    if (_sapp_ios_unthrottled_timer) {
        dispatch_source_cancel(_sapp_ios_unthrottled_timer);
        _sapp_ios_unthrottled_timer = nil;
    }
    if (interval == 0) {
        if (_sapp.mtl.display_link) {
            [_sapp.mtl.display_link setPaused:YES];
        }
        _sapp_ios_unthrottled_timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_main_queue());
        dispatch_source_set_timer(_sapp_ios_unthrottled_timer, DISPATCH_TIME_NOW, 100 * NSEC_PER_USEC, 100 * NSEC_PER_USEC);
        dispatch_source_set_event_handler(_sapp_ios_unthrottled_timer, ^{
            [_sapp.ios.view displayLinkFired:nil];
        });
        dispatch_resume(_sapp_ios_unthrottled_timer);
    } else {
        if (_sapp.mtl.display_link) {
            [_sapp.mtl.display_link setPaused:NO];
            if ([_sapp.mtl.display_link respondsToSelector:@selector(setPreferredFrameRateRange:)]) {
                NSInteger max_fps = 60;
                if (_sapp.ios.window && _sapp.ios.window.windowScene && _sapp.ios.window.windowScene.screen) {
                    max_fps = _sapp.ios.window.windowScene.screen.maximumFramesPerSecond;
                }
                const float preferred_fps = (float)max_fps / (float)interval;
                CAFrameRateRange frame_rate_range = { preferred_fps, preferred_fps, preferred_fps };
                _sapp.mtl.display_link.preferredFrameRateRange = frame_rate_range;
            }
        }
    }
    #endif
}
#endif /* _SAPP_IOS */

#if defined(_SAPP_WIN32)
typedef struct {
    int target_index;
    int index;
    bool found;
    bool fetch_name;
    HMONITOR h_display;
    RECT rect;
    char name[128];
} _sapp_win32_display_enum_t;

_SOKOL_PRIVATE BOOL CALLBACK _sapp_win32_display_enum_proc(HMONITOR h_monitor, HDC h_dc, LPRECT lprc_monitor, LPARAM dw_data) {
    (void)h_dc; (void)lprc_monitor;
    _sapp_win32_display_enum_t* data = (_sapp_win32_display_enum_t*)dw_data;
    if (data->h_display) {
        if (data->h_display == h_monitor) {
            data->found = true;
            MONITORINFO mi;
            memset(&mi, 0, sizeof(mi));
            mi.cbSize = sizeof(mi);
            GetMonitorInfo(h_monitor, &mi);
            data->rect = mi.rcMonitor;
            return FALSE;
        }
        data->index++;
    } else {
        if (data->index == data->target_index) {
            data->found = true;
            MONITORINFOEX mi;
            memset(&mi, 0, sizeof(mi));
            mi.cbSize = sizeof(mi);
            GetMonitorInfo(h_monitor, (MONITORINFO*)&mi);
            data->rect = mi.rcMonitor;
            if (data->fetch_name) {
                strncpy(data->name, mi.szDevice, sizeof(data->name));
                data->name[sizeof(data->name)-1] = 0;
            }
            return FALSE;
        }
        data->index++;
    }
    return TRUE;
}

_SOKOL_PRIVATE void _sapp_win32_set_window_position(int x, int y) {
  SetWindowPos(_sapp.win32.hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

_SOKOL_PRIVATE void _sapp_win32_set_window_size(int w, int h) {
  if (_sapp.win32.hwnd) {
    RECT rect = {0, 0, w, h};
    DWORD style = (DWORD)GetWindowLongPtr(_sapp.win32.hwnd, GWL_STYLE);
    DWORD ex_style = (DWORD)GetWindowLongPtr(_sapp.win32.hwnd, GWL_EXSTYLE);
    AdjustWindowRectEx(&rect, style, FALSE, ex_style);
    int win_w = rect.right - rect.left;
    int win_h = rect.bottom - rect.top;
    SetWindowPos(_sapp.win32.hwnd, NULL, 0, 0, win_w, win_h,
                 SWP_NOMOVE | SWP_NOZORDER);
  }
}

_SOKOL_PRIVATE void _sapp_win32_get_window_position(int *x, int *y) {
  if (_sapp.win32.hwnd) {
    RECT rect;
    GetWindowRect(_sapp.win32.hwnd, &rect);
    if (x)
      *x = (int)rect.left;
    if (y)
      *y = (int)rect.top;
  } else {
    if (x)
      *x = 0;
    if (y)
      *y = 0;
  }
}

_SOKOL_PRIVATE void _sapp_win32_get_window_size(int *w, int *h) {
  if (_sapp.win32.hwnd) {
    RECT rect;
    GetClientRect(_sapp.win32.hwnd, &rect);
    if (w)
      *w = (int)(rect.right - rect.left);
    if (h)
      *h = (int)(rect.bottom - rect.top);
  } else {
    if (w)
      *w = 0;
    if (h)
      *h = 0;
  }
}

_SOKOL_PRIVATE void _sapp_win32_set_mouse_position(float x, float y) {
  if (_sapp.win32.hwnd) {
    POINT position;
    position.x = (LONG)_sapp_roundf_gzero(x / _sapp.win32.dpi.mouse_scale);
    position.y = (LONG)_sapp_roundf_gzero(y / _sapp.win32.dpi.mouse_scale);
    if (ClientToScreen(_sapp.win32.hwnd, &position)) {
      SetCursorPos(position.x, position.y);
    }
  }
}

_SOKOL_PRIVATE bool _sapp_win32_window_focused(void) {
  if (_sapp.win32.hwnd) {
    return GetForegroundWindow() == _sapp.win32.hwnd;
  }
  return false;
}

_SOKOL_PRIVATE int _sapp_win32_num_displays(void) {
  return GetSystemMetrics(SM_CMONITORS);
}

_SOKOL_PRIVATE int _sapp_win32_display_width(int index) {
  _sapp_win32_display_enum_t data;
  ZeroMemory(&data, sizeof(data));
  data.target_index = index;
  EnumDisplayMonitors(NULL, NULL, _sapp_win32_display_enum_proc, (LPARAM)&data);
  if (data.found) {
    return (int)(data.rect.right - data.rect.left);
  }
  return 0;
}

_SOKOL_PRIVATE int _sapp_win32_display_height(int index) {
  _sapp_win32_display_enum_t data;
  ZeroMemory(&data, sizeof(data));
  data.target_index = index;
  EnumDisplayMonitors(NULL, NULL, _sapp_win32_display_enum_proc, (LPARAM)&data);
  if (data.found) {
    return (int)(data.rect.bottom - data.rect.top);
  }
  return 0;
}

_SOKOL_PRIVATE int _sapp_win32_current_display(void) {
  if (_sapp.win32.hwnd) {
    HMONITOR hDisplay =
        MonitorFromWindow(_sapp.win32.hwnd, MONITOR_DEFAULTTONEAREST);
    _sapp_win32_display_enum_t data;
    ZeroMemory(&data, sizeof(data));
    data.h_display = hDisplay;
    EnumDisplayMonitors(NULL, NULL, _sapp_win32_display_enum_proc, (LPARAM)&data);
    if (data.found)
      return data.index;
  }
  return 0;
}

_SOKOL_PRIVATE void _sapp_win32_set_display(int index) {
  if (_sapp.win32.hwnd) {
    _sapp_win32_display_enum_t data;
    ZeroMemory(&data, sizeof(data));
    data.target_index = index;
    EnumDisplayMonitors(NULL, NULL, _sapp_win32_display_enum_proc, (LPARAM)&data);
    if (data.found) {
      RECT r = data.rect;
      if (_sapp.fullscreen) {
        /* If already fullscreen, just move the borderless window to the new monitor */
        const int w = r.right - r.left;
        const int h = r.bottom - r.top;
        SetWindowPos(_sapp.win32.hwnd, HWND_TOP, r.left, r.top, w, h, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
        /* Also update the stored rect so exiting fullscreen stays on the new monitor */
        _sapp.win32.stored_window_rect.left += r.left;
        _sapp.win32.stored_window_rect.top += r.top;
        _sapp.win32.stored_window_rect.right += r.left;
        _sapp.win32.stored_window_rect.bottom += r.top;
      } else {
        RECT win_rect;
        GetWindowRect(_sapp.win32.hwnd, &win_rect);
        int w = win_rect.right - win_rect.left;
        int h = win_rect.bottom - win_rect.top;
        int x = r.left + (r.right - r.left - w) / 2;
        int y = r.top + (r.bottom - r.top - h) / 2;
        SetWindowPos(_sapp.win32.hwnd, NULL, x, y, 0, 0,
                     SWP_NOSIZE | SWP_NOZORDER);
      }
    }
  }
}

_SOKOL_PRIVATE const char* _sapp_win32_display_name(int index) {
  _sapp_win32_display_enum_t data;
  ZeroMemory(&data, sizeof(data));
  data.target_index = index;
  data.fetch_name = true;
  EnumDisplayMonitors(NULL, NULL, _sapp_win32_display_enum_proc, (LPARAM)&data);
  if (data.found) {
    static char buf[128];
    strncpy(buf, data.name, sizeof(buf));
    buf[sizeof(buf) - 1] = 0;
    return buf;
  }
  return "";
}

_SOKOL_PRIVATE void _sapp_win32_set_swap_interval(int interval) {
    _sapp.desc.swap_interval = interval;
    #if defined(SOKOL_GLCORE)
    typedef BOOL (WINAPI * PFNWGLSWAPINTERVALEXTPROC) (int interval);
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
    if (wglSwapIntervalEXT) {
        wglSwapIntervalEXT(interval);
    }
    #endif
    #if defined(SOKOL_VULKAN)
    _sapp_vk_recreate_swapchain();
    #endif
}
#endif /* _SAPP_WIN32 */

#if defined(_SAPP_LINUX)
#include <X11/extensions/Xrandr.h>
_SOKOL_PRIVATE void _sapp_x11_set_window_position(int x, int y) {
  XMoveWindow(_sapp.x11.display, _sapp.x11.window, x, y);
}

_SOKOL_PRIVATE void _sapp_x11_set_window_size(int w, int h) {
  XResizeWindow(_sapp.x11.display, _sapp.x11.window, (unsigned int)w,
                (unsigned int)h);
}

_SOKOL_PRIVATE void _sapp_x11_get_window_position(int *x, int *y) {
  if (_sapp.x11.display) {
    Window child;
    XTranslateCoordinates(_sapp.x11.display, _sapp.x11.window,
                          DefaultRootWindow(_sapp.x11.display), 0, 0, x, y,
                          &child);
  } else {
    if (x)
      *x = 0;
    if (y)
      *y = 0;
  }
}

_SOKOL_PRIVATE void _sapp_x11_get_window_size(int *w, int *h) {
  if (_sapp.x11.display) {
    XWindowAttributes attribs;
    XGetWindowAttributes(_sapp.x11.display, _sapp.x11.window, &attribs);
    if (w)
      *w = attribs.width;
    if (h)
      *h = attribs.height;
  } else {
    if (w)
      *w = 0;
    if (h)
      *h = 0;
  }
}

_SOKOL_PRIVATE void _sapp_x11_set_mouse_position(float x, float y) {
  if (_sapp.x11.display) {
    XWarpPointer(_sapp.x11.display, None, _sapp.x11.window, 0, 0, 0, 0,
                 _sapp_roundf_gzero(x), _sapp_roundf_gzero(y));
    XFlush(_sapp.x11.display);
  }
}

/* [libwgrender] _sapp_x11_window_focused() removed: sokol_app.h defines it now (it tracks
   focus from FocusIn/FocusOut events). */

_SOKOL_PRIVATE int _sapp_x11_display_width(int index) {
  if (!_sapp.x11.display) {
    return 0;
  }
  int num_displays = 0;
  int width = 0;
  XRRMonitorInfo *displays = XRRGetMonitors(
      _sapp.x11.display, DefaultRootWindow(_sapp.x11.display), True, &num_displays);
  if (displays) {
    if (index >= 0 && index < num_displays) {
      width = displays[index].width;
    }
    XRRFreeMonitors(displays);
  } else if (index == 0) {
    width = DisplayWidth(_sapp.x11.display, DefaultScreen(_sapp.x11.display));
  }
  return width;
}

_SOKOL_PRIVATE int _sapp_x11_display_height(int index) {
  if (!_sapp.x11.display) {
    return 0;
  }
  int num_displays = 0;
  int height = 0;
  XRRMonitorInfo *displays = XRRGetMonitors(
      _sapp.x11.display, DefaultRootWindow(_sapp.x11.display), True, &num_displays);
  if (displays) {
    if (index >= 0 && index < num_displays) {
      height = displays[index].height;
    }
    XRRFreeMonitors(displays);
  } else if (index == 0) {
    height = DisplayHeight(_sapp.x11.display, DefaultScreen(_sapp.x11.display));
  }
  return height;
}

_SOKOL_PRIVATE int _sapp_x11_num_displays(void) {
  if (!_sapp.x11.display) {
    return 1;
  }
  int num_displays = 0;
  XRRMonitorInfo *displays = XRRGetMonitors(
      _sapp.x11.display, DefaultRootWindow(_sapp.x11.display), True, &num_displays);
  if (displays) {
    XRRFreeMonitors(displays);
  }
  return num_displays > 0 ? num_displays : 1;
}

_SOKOL_PRIVATE int _sapp_x11_current_display(void) {
  if (!_sapp.x11.display) {
    return 0;
  }
  int num_displays = 0;
  XRRMonitorInfo *displays = XRRGetMonitors(
      _sapp.x11.display, DefaultRootWindow(_sapp.x11.display), True, &num_displays);
  if (displays) {
    int x, y;
    Window child;
    XTranslateCoordinates(_sapp.x11.display, _sapp.x11.window,
                          DefaultRootWindow(_sapp.x11.display), 0, 0, &x, &y,
                          &child);
    XWindowAttributes attribs;
    XGetWindowAttributes(_sapp.x11.display, _sapp.x11.window, &attribs);
    int cx = x + attribs.width / 2;
    int cy = y + attribs.height / 2;

    int result = 0;
    for (int i = 0; i < num_displays; i++) {
      if (cx >= displays[i].x && cx < (displays[i].x + displays[i].width) &&
          cy >= displays[i].y && cy < (displays[i].y + displays[i].height)) {
        result = i;
        break;
      }
    }
    XRRFreeMonitors(displays);
    return result;
  }
  return 0;
}

_SOKOL_PRIVATE void _sapp_x11_set_display(int index) {
  if (!_sapp.x11.display) {
    return;
  }
  int num_displays = 0;
  XRRMonitorInfo *displays = XRRGetMonitors(
      _sapp.x11.display, DefaultRootWindow(_sapp.x11.display), True, &num_displays);
  if (displays) {
    if (index >= 0 && index < num_displays) {
      if (_sapp.fullscreen) {
        /* If already fullscreen, move and resize to new monitor */
        XMoveResizeWindow(_sapp.x11.display, _sapp.x11.window,
                          displays[index].x, displays[index].y,
                          (unsigned int)displays[index].width, (unsigned int)displays[index].height);
      } else {
        XWindowAttributes attribs;
        XGetWindowAttributes(_sapp.x11.display, _sapp.x11.window, &attribs);
        int x = displays[index].x + (displays[index].width - attribs.width) / 2;
        int y = displays[index].y + (displays[index].height - attribs.height) / 2;
        XMoveWindow(_sapp.x11.display, _sapp.x11.window, x, y);
      }
      XFlush(_sapp.x11.display);
    }
    XRRFreeMonitors(displays);
  }
}

_SOKOL_PRIVATE const char* _sapp_x11_display_name(int index) {
  if (!_sapp.x11.display) {
    return "";
  }
  int num_displays = 0;
  XRRMonitorInfo* displays = XRRGetMonitors(
      _sapp.x11.display, DefaultRootWindow(_sapp.x11.display), True, &num_displays);
  if (displays) {
    if (index >= 0 && index < num_displays) {
      char* name = XGetAtomName(_sapp.x11.display, displays[index].name);
      static char buf[256];
      if (name) {
        strncpy(buf, name, sizeof(buf));
        buf[sizeof(buf) - 1] = 0;
        XFree(name);
        XRRFreeMonitors(displays);
        return buf;
      }
    }
    XRRFreeMonitors(displays);
  }
  return "";
}

_SOKOL_PRIVATE void _sapp_linux_set_swap_interval(int interval) {
    _sapp.desc.swap_interval = interval;
    #if defined(_SAPP_GLX)
    /* [libwgrender] use sokol_app's GLX loader: glXGetProcAddress isn't declared (sokol
       loads libGL dynamically), and the drawable is the GLX window, not the X11 one */
    _sapp_glx_swapinterval(interval);
    #elif defined(_SAPP_EGL)
    eglSwapInterval(_sapp.egl.display, interval);
    #endif
    #if defined(SOKOL_VULKAN)
    _sapp_vk_recreate_swapchain();
    #endif
}
#endif /* _SAPP_LINUX */

#if defined(_SAPP_EMSCRIPTEN)
#include <emscripten.h>
#endif

SOKOL_API_IMPL void sapp_set_window_position(int x, int y) {
#if defined(_SAPP_MACOS)
  _sapp_macos_set_window_position(x, y);
#elif defined(_SAPP_WIN32)
  _sapp_win32_set_window_position(x, y);
#elif defined(_SAPP_LINUX)
  _sapp_x11_set_window_position(x, y);
#endif
}

SOKOL_API_IMPL void sapp_set_window_size(int w, int h) {
#if defined(_SAPP_MACOS)
  _sapp_macos_set_window_size(w, h);
#elif defined(_SAPP_WIN32)
  _sapp_win32_set_window_size(w, h);
#elif defined(_SAPP_LINUX)
  _sapp_x11_set_window_size(w, h);
#endif
}

SOKOL_API_IMPL void sapp_get_window_position(int *x, int *y) {
#if defined(_SAPP_MACOS)
  _sapp_macos_get_window_position(x, y);
#elif defined(_SAPP_WIN32)
  _sapp_win32_get_window_position(x, y);
#elif defined(_SAPP_LINUX)
  _sapp_x11_get_window_position(x, y);
#endif
}

SOKOL_API_IMPL void sapp_get_window_size(int *w, int *h) {
#if defined(_SAPP_MACOS)
  _sapp_macos_get_window_size(w, h);
#elif defined(_SAPP_WIN32)
  _sapp_win32_get_window_size(w, h);
#elif defined(_SAPP_LINUX)
  _sapp_x11_get_window_size(w, h);
#endif
}

SOKOL_API_IMPL void sapp_set_mouse_position(float x, float y) {
  if (_sapp.mouse.locked) {
    return;
  }
#if defined(_SAPP_MACOS)
  _sapp_macos_set_mouse_position(x, y);
#elif defined(_SAPP_WIN32)
  _sapp_win32_set_mouse_position(x, y);
#elif defined(_SAPP_LINUX)
  _sapp_x11_set_mouse_position(x, y);
#else
  (void)x;
  (void)y;
#endif
}

SOKOL_API_IMPL void sapp_get_mouse_position(float *x, float *y) {
  if (x)
    *x = _sapp.mouse.x;
  if (y)
    *y = _sapp.mouse.y;
}

SOKOL_API_IMPL bool sapp_window_focused(void) {
#if defined(_SAPP_MACOS)
  return _sapp_macos_window_focused();
#elif defined(_SAPP_WIN32)
  return _sapp_win32_window_focused();
#elif defined(_SAPP_LINUX)
  return _sapp_x11_window_focused();
#else
  return false;
#endif
}

SOKOL_API_IMPL int sapp_display_width(int index) {
#if defined(_SAPP_MACOS)
  return _sapp_macos_display_width(index);
#elif defined(_SAPP_WIN32)
  return _sapp_win32_display_width(index);
#elif defined(_SAPP_LINUX)
  return _sapp_x11_display_width(index);
#else
  (void)index;
  return 0;
#endif
}

SOKOL_API_IMPL int sapp_display_height(int index) {
#if defined(_SAPP_MACOS)
  return _sapp_macos_display_height(index);
#elif defined(_SAPP_WIN32)
  return _sapp_win32_display_height(index);
#elif defined(_SAPP_LINUX)
  return _sapp_x11_display_height(index);
#else
  (void)index;
  return 0;
#endif
}

SOKOL_API_IMPL int sapp_num_displays(void) {
#if defined(_SAPP_MACOS)
  return _sapp_macos_num_displays();
#elif defined(_SAPP_WIN32)
  return _sapp_win32_num_displays();
#elif defined(_SAPP_LINUX)
  return _sapp_x11_num_displays();
#else
  return 1;
#endif
}

SOKOL_API_IMPL int sapp_current_display(void) {
#if defined(_SAPP_MACOS)
  return _sapp_macos_current_display();
#elif defined(_SAPP_WIN32)
  return _sapp_win32_current_display();
#elif defined(_SAPP_LINUX)
  return _sapp_x11_current_display();
#else
  return 0;
#endif
}

SOKOL_API_IMPL void sapp_set_display(int index) {
#if defined(_SAPP_MACOS)
  _sapp_macos_set_display(index);
#elif defined(_SAPP_WIN32)
  _sapp_win32_set_display(index);
#elif defined(_SAPP_LINUX)
  _sapp_x11_set_display(index);
#endif
}

SOKOL_API_IMPL const char* sapp_display_name(int index) {
#if defined(_SAPP_MACOS)
  return _sapp_macos_display_name(index);
#elif defined(_SAPP_WIN32)
  return _sapp_win32_display_name(index);
#elif defined(_SAPP_LINUX)
  return _sapp_x11_display_name(index);
#else
  (void)index;
  return "";
#endif
}

SOKOL_API_IMPL void sapp_set_fullscreen(bool enable) {
  if (_sapp.fullscreen != enable) {
#if defined(_SAPP_WIN32)
    _sapp_win32_set_fullscreen(enable, SWP_SHOWWINDOW);
#elif defined(_SAPP_LINUX)
    /* as sokol_app's _sapp_x11_toggle_fullscreen(): the flag sapp_is_fullscreen() reads,
       the window manager asked, and the dimensions updated */
    _sapp.fullscreen = enable;
    _sapp_x11_set_fullscreen(enable);
    _sapp_x11_update_dimensions_from_window_size();
#elif defined(_SAPP_MACOS)
    _sapp_macos_toggle_fullscreen();
#elif defined(_SAPP_EMSCRIPTEN)
    _sapp_emsc_toggle_fullscreen();
#endif
  }
}

SOKOL_API_IMPL void sapp_set_swap_interval(int interval) {
#if defined(_SAPP_MACOS)
  _sapp_macos_set_swap_interval(interval);
#elif defined(_SAPP_IOS)
  _sapp_ios_set_swap_interval(interval);
#elif defined(_SAPP_WIN32)
  _sapp_win32_set_swap_interval(interval);
#elif defined(_SAPP_LINUX)
  _sapp_linux_set_swap_interval(interval);
#elif defined(_SAPP_ANDROID)
  _sapp.desc.swap_interval = interval;
  #if defined(_SAPP_EGL)
  eglSwapInterval(_sapp.egl.display, interval);
  #endif
#elif defined(_SAPP_EMSCRIPTEN)
  _sapp.desc.swap_interval = interval;
  emscripten_set_main_loop_timing(interval == 0 ? EM_TIMING_SETTIMEOUT : EM_TIMING_RAF, interval);
#else
  _sapp.desc.swap_interval = interval;
#endif
}

SOKOL_API_IMPL int sapp_get_swap_interval(void) {
  return _sapp.desc.swap_interval;
}

#endif /* SOKOL_APP_UTILS_IMPL */
