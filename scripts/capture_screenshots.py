"""Capture CleanerQt window screenshots using Win32 API + Pillow."""
from __future__ import annotations

import ctypes
import ctypes.wintypes
import time
from pathlib import Path

from PIL import ImageGrab

user32 = ctypes.windll.user32

OUT_DIR = Path(__file__).resolve().parents[1] / "docs" / "screenshots"
OUT_DIR.mkdir(parents=True, exist_ok=True)

MOUSEEVENTF_LEFTDOWN = 0x0002
MOUSEEVENTF_LEFTUP = 0x0004


def click_screen(x: int, y: int) -> None:
    user32.SetCursorPos(x, y)
    time.sleep(0.1)
    user32.mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0)
    user32.mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0)
    time.sleep(0.4)


def find_cleaner_hwnd() -> int:
    result = []

    @ctypes.WINFUNCTYPE(ctypes.c_bool, ctypes.c_void_p, ctypes.c_void_p)
    def enum_proc(hwnd, _):
        if not user32.IsWindowVisible(hwnd):
            return True
        length = user32.GetWindowTextLengthW(hwnd) + 1
        buf = ctypes.create_unicode_buffer(length)
        user32.GetWindowTextW(hwnd, buf, length)
        title = buf.value
        if "v2.0" in title or "Disk Space" in title or "磁盘空间" in title:
            result.append(hwnd)
            return False
        return True

    user32.EnumWindows(enum_proc, 0)
    if not result:
        raise RuntimeError("CleanerQt window not found")
    return result[0]


def window_rect(hwnd: int) -> tuple[int, int, int, int]:
    rect = ctypes.wintypes.RECT()
    user32.GetWindowRect(hwnd, ctypes.byref(rect))
    return rect.left, rect.top, rect.right, rect.bottom


def capture(hwnd: int, name: str) -> Path:
    user32.SetForegroundWindow(hwnd)
    time.sleep(0.6)
    left, top, right, bottom = window_rect(hwnd)
    img = ImageGrab.grab(bbox=(left, top, right, bottom))
    path = OUT_DIR / f"{name}.png"
    img.save(path)
    print(f"Saved {path} ({path.stat().st_size} bytes)")
    return path


def click_tab(hwnd: int, index: int) -> None:
    left, top, right, bottom = window_rect(hwnd)
    width = right - left
    # Main TabBar sits below toolbar (~200px from top of client area)
    tab_y = top + 200
    tab_width = width / 5
    tab_x = int(left + tab_width * index + tab_width / 2)
    user32.SetForegroundWindow(hwnd)
    time.sleep(0.2)
    click_screen(tab_x, tab_y)


def main() -> None:
    hwnd = find_cleaner_hwnd()
    click_tab(hwnd, 0)
    capture(hwnd, "treemap-overview")
    click_tab(hwnd, 1)
    capture(hwnd, "all-files")
    click_tab(hwnd, 3)
    capture(hwnd, "cleanup-hub")
    click_tab(hwnd, 0)
    capture(hwnd, "dark-theme")


if __name__ == "__main__":
    main()
