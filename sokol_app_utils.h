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
/* returns the width of the primary screen in pixels */
SOKOL_APP_UTILS_API_DECL int sapp_screen_width(void);
/* returns the height of the primary screen in pixels */
SOKOL_APP_UTILS_API_DECL int sapp_screen_height(void);
/* returns true if the window has focus */
SOKOL_APP_UTILS_API_DECL bool sapp_window_focused(void);
/* returns the number of connected monitors (only on desktop platforms) */
SOKOL_APP_UTILS_API_DECL int sapp_num_monitors(void);
/* get the current monitor index (only on desktop platforms) */
SOKOL_APP_UTILS_API_DECL int sapp_current_monitor(void);
/* set the current monitor (only on desktop platforms) */
SOKOL_APP_UTILS_API_DECL void sapp_set_monitor(int index);
/* sets fullscreen mode (wrapper around platform specific logic) */
SOKOL_APP_UTILS_API_DECL void sapp_set_fullscreen(bool enable);

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

_SOKOL_PRIVATE bool _sapp_macos_window_focused(void) {
  if (_sapp.macos.window) {
    return [_sapp.macos.window isKeyWindow];
  }
  return false;
}

_SOKOL_PRIVATE int _sapp_macos_screen_width(void) {
  NSArray *screens = [NSScreen screens];
  if (screens && [screens count] > 0) {
    NSScreen *primary = [screens objectAtIndex:0];
    return (int)[primary frame].size.width;
  }
  return 0;
}

_SOKOL_PRIVATE int _sapp_macos_screen_height(void) {
  NSArray *screens = [NSScreen screens];
  if (screens && [screens count] > 0) {
    NSScreen *primary = [screens objectAtIndex:0];
    return (int)[primary frame].size.height;
  }
  return 0;
}

_SOKOL_PRIVATE int _sapp_macos_num_monitors(void) {
  NSArray *screens = [NSScreen screens];
  return (int)[screens count];
}

_SOKOL_PRIVATE int _sapp_macos_current_monitor(void) {
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

_SOKOL_PRIVATE void _sapp_macos_set_monitor(int index) {
  if (_sapp.macos.window) {
    NSArray *screens = [NSScreen screens];
    if ((index >= 0) && (index < (int)[screens count])) {
      NSScreen *screen = [screens objectAtIndex:index];
      NSRect screen_frame = [screen frame];
      NSRect window_frame = [_sapp.macos.window frame];
      CGFloat x = screen_frame.origin.x +
                  (screen_frame.size.width - window_frame.size.width) / 2.0f;
      CGFloat y = screen_frame.origin.y +
                  (screen_frame.size.height - window_frame.size.height) / 2.0f;
      [_sapp.macos.window setFrameOrigin:NSMakePoint(x, y)];
    }
  }
}
#endif /* _SAPP_MACOS */

#if defined(_SAPP_WIN32)
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

_SOKOL_PRIVATE bool _sapp_win32_window_focused(void) {
  if (_sapp.win32.hwnd) {
    return GetForegroundWindow() == _sapp.win32.hwnd;
  }
  return false;
}

_SOKOL_PRIVATE int _sapp_win32_screen_width(void) {
  if (_sapp.win32.hwnd) {
    HMONITOR hMonitor =
        MonitorFromWindow(_sapp.win32.hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi;
    ZeroMemory(&mi, sizeof(mi));
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfo(hMonitor, &mi)) {
      return (mi.rcMonitor.right - mi.rcMonitor.left) > 0
                 ? (mi.rcMonitor.right - mi.rcMonitor.left)
                 : 1;
    }
  }
  return GetSystemMetrics(SM_CXSCREEN);
}

_SOKOL_PRIVATE int _sapp_win32_screen_height(void) {
  if (_sapp.win32.hwnd) {
    HMONITOR hMonitor =
        MonitorFromWindow(_sapp.win32.hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi;
    ZeroMemory(&mi, sizeof(mi));
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfo(hMonitor, &mi)) {
      return (mi.rcMonitor.bottom - mi.rcMonitor.top) > 0
                 ? (mi.rcMonitor.bottom - mi.rcMonitor.top)
                 : 1;
    }
  }
  return GetSystemMetrics(SM_CYSCREEN);
}

typedef struct {
  HMONITOR h_mon;
  int index;
  int target_index;
  RECT rect;
  bool found;
} _sapp_win32_mon_enum_t;

_SOKOL_PRIVATE BOOL CALLBACK _sapp_win32_mon_enum_proc(HMONITOR hMonitor,
                                                       HDC hdc, LPRECT lprc,
                                                       LPARAM dwData) {
  _sapp_win32_mon_enum_t *data = (_sapp_win32_mon_enum_t *)dwData;
  (void)hdc;
  /* Mode 1: Find index of h_mon */
  if (data->h_mon) {
    if (data->h_mon == hMonitor) {
      data->found = true;
      return FALSE;
    }
    data->index++;
  }
  /* Mode 2: Find rect of target_index */
  else {
    if (data->index == data->target_index) {
      data->rect = *lprc;
      data->found = true;
      return FALSE;
    }
    data->index++;
  }
  return TRUE;
}

_SOKOL_PRIVATE int _sapp_win32_num_monitors(void) {
  return GetSystemMetrics(SM_CMONITORS);
}

_SOKOL_PRIVATE int _sapp_win32_current_monitor(void) {
  if (_sapp.win32.hwnd) {
    HMONITOR hMon =
        MonitorFromWindow(_sapp.win32.hwnd, MONITOR_DEFAULTTONEAREST);
    _sapp_win32_mon_enum_t data;
    ZeroMemory(&data, sizeof(data));
    data.h_mon = hMon;
    EnumDisplayMonitors(NULL, NULL, _sapp_win32_mon_enum_proc, (LPARAM)&data);
    if (data.found)
      return data.index;
  }
  return 0;
}

_SOKOL_PRIVATE void _sapp_win32_set_monitor(int index) {
  if (_sapp.win32.hwnd) {
    _sapp_win32_mon_enum_t data;
    ZeroMemory(&data, sizeof(data));
    data.target_index = index;
    EnumDisplayMonitors(NULL, NULL, _sapp_win32_mon_enum_proc, (LPARAM)&data);
    if (data.found) {
      RECT r = data.rect;
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
#endif /* _SAPP_WIN32 */

#if defined(_SAPP_LINUX)
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

_SOKOL_PRIVATE bool _sapp_x11_window_focused(void) {
  if (_sapp.x11.display) {
    Window focused;
    int revert_to;
    XGetInputFocus(_sapp.x11.display, &focused, &revert_to);
    return focused == _sapp.x11.window;
  }
  return false;
}

_SOKOL_PRIVATE int _sapp_x11_screen_width(void) {
  return DisplayWidth(_sapp.x11.display, DefaultScreen(_sapp.x11.display));
}

_SOKOL_PRIVATE int _sapp_x11_screen_height(void) {
  return DisplayHeight(_sapp.x11.display, DefaultScreen(_sapp.x11.display));
}

_SOKOL_PRIVATE int _sapp_x11_num_monitors(void) {
  int num_monitors = 0;
  XRRMonitorInfo *monitors = XRRGetMonitors(
      _sapp.x11.display, DefaultRootWindow(_sapp.x11.display), True, &num_monitors);
  if (monitors) {
    XRRFreeMonitors(monitors);
  }
  return num_monitors > 0 ? num_monitors : 1;
}

_SOKOL_PRIVATE int _sapp_x11_current_monitor(void) {
  int num_monitors = 0;
  XRRMonitorInfo *monitors = XRRGetMonitors(
      _sapp.x11.display, DefaultRootWindow(_sapp.x11.display), True, &num_monitors);
  if (monitors) {
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
    for (int i = 0; i < num_monitors; i++) {
      if (cx >= monitors[i].x && cx < (monitors[i].x + monitors[i].width) &&
          cy >= monitors[i].y && cy < (monitors[i].y + monitors[i].height)) {
        result = i;
        break;
      }
    }
    XRRFreeMonitors(monitors);
    return result;
  }
  return 0;
}

_SOKOL_PRIVATE void _sapp_x11_set_monitor(int index) {
  int num_monitors = 0;
  XRRMonitorInfo *monitors = XRRGetMonitors(
      _sapp.x11.display, DefaultRootWindow(_sapp.x11.display), True, &num_monitors);
  if (monitors) {
    if (index >= 0 && index < num_monitors) {
      XWindowAttributes attribs;
      XGetWindowAttributes(_sapp.x11.display, _sapp.x11.window, &attribs);
      int x = monitors[index].x + (monitors[index].width - attribs.width) / 2;
      int y = monitors[index].y + (monitors[index].height - attribs.height) / 2;
      XMoveWindow(_sapp.x11.display, _sapp.x11.window, x, y);
    }
    XRRFreeMonitors(monitors);
  }
}
#endif /* _SAPP_LINUX */

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

SOKOL_API_IMPL int sapp_screen_width(void) {
#if defined(_SAPP_MACOS)
  return _sapp_macos_screen_width();
#elif defined(_SAPP_WIN32)
  return _sapp_win32_screen_width();
#elif defined(_SAPP_LINUX)
  return _sapp_x11_screen_width();
#else
  return 0;
#endif
}

SOKOL_API_IMPL int sapp_screen_height(void) {
#if defined(_SAPP_MACOS)
  return _sapp_macos_screen_height();
#elif defined(_SAPP_WIN32)
  return _sapp_win32_screen_height();
#elif defined(_SAPP_LINUX)
  return _sapp_x11_screen_height();
#else
  return 0;
#endif
}

SOKOL_API_IMPL int sapp_num_monitors(void) {
#if defined(_SAPP_MACOS)
  return _sapp_macos_num_monitors();
#elif defined(_SAPP_WIN32)
  return _sapp_win32_num_monitors();
#elif defined(_SAPP_LINUX)
  return _sapp_x11_num_monitors();
#else
  return 1;
#endif
}

SOKOL_API_IMPL int sapp_current_monitor(void) {
#if defined(_SAPP_MACOS)
  return _sapp_macos_current_monitor();
#elif defined(_SAPP_WIN32)
  return _sapp_win32_current_monitor();
#elif defined(_SAPP_LINUX)
  return _sapp_x11_current_monitor();
#else
  return 0;
#endif
}

SOKOL_API_IMPL void sapp_set_monitor(int index) {
#if defined(_SAPP_MACOS)
  _sapp_macos_set_monitor(index);
#elif defined(_SAPP_WIN32)
  _sapp_win32_set_monitor(index);
#elif defined(_SAPP_LINUX)
  _sapp_x11_set_monitor(index);
#endif
}

SOKOL_API_IMPL void sapp_set_fullscreen(bool enable) {
  if (_sapp.fullscreen != enable) {
#if defined(_SAPP_WIN32)
    _sapp_win32_set_fullscreen(enable, SWP_SHOWWINDOW);
#elif defined(_SAPP_LINUX)
    _sapp_x11_set_fullscreen(enable);
#elif defined(_SAPP_MACOS)
    _sapp_macos_toggle_fullscreen();
#elif defined(_SAPP_EMSCRIPTEN)
    _sapp_emsc_toggle_fullscreen();
#endif
  }
}

#endif /* SOKOL_APP_UTILS_IMPL */
