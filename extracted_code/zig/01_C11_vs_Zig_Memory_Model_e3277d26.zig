//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
// Line: 2157
// Language: zig
// Block ID: e3277d26
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 快速参考
const Atomic = std.atomic.Value;

// 声明
var a = Atomic(i32).init(0);

// 基本操作
const v = a.load(.SeqCst);
a.store(42, .SeqCst);
const old = a.swap(100, .SeqCst);
const prev = a.fetchAdd(1, .SeqCst);

// 显式内存序
a.store(1, .Release);
const x = a.load(.Acquire);

// CAS
const result = a.tryCmpxchg(0, 1, .AcqRel, .Monotonic);
const success = (result == null);

// 栅栏
std.atomic.fence(.SeqCst);

// 提示
std.atomic.spinLoopHint();
