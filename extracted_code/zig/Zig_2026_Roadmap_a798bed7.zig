//
// Auto-generated from: Zig\2026_latest\Zig_2026_Roadmap.md
// Line: 594
// Language: zig
// Block ID: a798bed7
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 2026: 可选的形式化验证

const verify = @import("std").verify;

// 函数契约
fn binarySearch(arr: []const i32, target: i32) ?usize
    // 前置条件
    requires arr.len > 0
    requires isSorted(arr)
    // 后置条件
    ensures return == null or arr[return.?] == target
{
    var left: usize = 0;
    var right = arr.len;

    while (left < right) {
        const mid = left + (right - left) / 2;

        verify.invariant(left <= right);  // 循环不变式
        verify.invariant(mid < arr.len);

        if (arr[mid] == target) return mid;
        if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    return null;
}

// 内存安全证明
fn safeArrayAccess(arr: []i32, index: usize) i32
    requires index < arr.len
    ensures return == arr[index]
{
    return arr[index];  // 编译器证明不会越界
}
