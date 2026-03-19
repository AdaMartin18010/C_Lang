//
// Auto-generated from: Zig\Zig_Formal_Semantics.md
// Line: 858
// Language: zig
// Block ID: e7f20b33
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 不同内存序的使用场景

fn memoryOrderExamples() void {
    var flag = std.atomic.Value(bool).init(false);
    var data: i32 = 0;

    // 场景 1: 简单的标志位 (不需要同步其他数据)
    flag.store(true, .monotonic);
    _ = flag.load(.monotonic);

    // 场景 2: 生产者-消费者 (需要同步数据)
    // 生产者线程
    data = 42;
    flag.store(true, .release);  // 保证 data=42 先执行

    // 消费者线程
    while (!flag.load(.acquire)) {}  // 保证看到 data=42
    std.debug.assert(data == 42);

    // 场景 3: 初始化标志 (只用一次)
    var initialized = std.atomic.Value(bool).init(false);
    if (!initialized.swap(true, .acq_rel)) {
        // 执行初始化 (只有一个线程会进入)
    }

    // 场景 4: 顺序一致性 (最强保证，但最慢)
    var seq_counter = std.atomic.Value(i32).init(0);
    _ = seq_counter.fetchAdd(1, .seq_cst);
    // 所有线程以相同顺序看到操作
}
