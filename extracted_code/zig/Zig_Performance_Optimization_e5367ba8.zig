//
// Auto-generated from: Zig\Zig_Performance_Optimization.md
// Line: 216
// Language: zig
// Block ID: e5367ba8
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 查找算法对比

// O(n) 线性查找
pub fn linearSearch(haystack: []const i32, needle: i32) ?usize {
    for (haystack, 0..) |item, i| {
        if (item == needle) return i;
    }
    return null;
}

// O(log n) 二分查找 (要求已排序)
pub fn binarySearch(haystack: []const i32, needle: i32) ?usize {
    var left: usize = 0;
    var right = haystack.len;

    while (left < right) {
        const mid = left + (right - left) / 2;
        if (haystack[mid] == needle) return mid;
        if (haystack[mid] < needle) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return null;
}

// O(1) 哈希查找
pub fn HashSet(comptime T: type) type {
    return std.HashMap(T, void, std.hash_map.defaultContext(T), 80);
}
