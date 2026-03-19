//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\02_Windows_ABI_Compatibility.md
// Line: 508
// Language: zig
// Block ID: 36c48f78
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码: Windows 调用约定与回调

// Windows 回调函数类型
const WNDPROC = *const fn (
    hwnd: HWND,
    uMsg: u32,
    wParam: WPARAM,
    lParam: LPARAM,
) callconv(.C) LRESULT;

// 实现 Windows 回调
fn windowProc(
    hwnd: HWND,
    uMsg: u32,
    wParam: WPARAM,
    lParam: LPARAM,
) callconv(.C) LRESULT {
    switch (uMsg) {
        0x0010 => { // WM_CLOSE
            return 0;
        },
        else => {
            // 调用默认处理
            return DefWindowProcA(hwnd, uMsg, wParam, lParam);
        },
    }
}

extern "user32" fn DefWindowProcA(
    hWnd: HWND,
    Msg: u32,
    wParam: WPARAM,
    lParam: LPARAM,
) callconv(.C) LRESULT;
