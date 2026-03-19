/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 2857
 * Language: c
 * Block ID: 048a827a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 优化前: 结构体填充过多
 */
struct Padded {
    char  c1;      // +0, size 1
    // 3字节填充
    int   i1;      // +4, size 4
    char  c2;      // +8, size 1
    // 3字节填充
    int   i2;      // +12, size 4
    short s1;      // +16, size 2
    // 6字节填充
    double d1;     // +24, size 8
};  // 总大小: 32字节，填充: 12字节

/*
 * 优化后: 按大小降序排列
 */
struct Optimized {
    double d1;     // +0, size 8
    int    i1;     // +8, size 4
    int    i2;     // +12, size 4
    short  s1;     // +16, size 2
    char   c1;     // +18, size 1
    char   c2;     // +19, size 1
    // 4字节填充以对齐到8
};  // 总大小: 24字节，填充: 4字节

/*
 * 优化效果:
 * - 节省: 8字节 (25%)
 * - 相同数据可放入更少缓存行
 * - 批量处理时性能提升
 */
