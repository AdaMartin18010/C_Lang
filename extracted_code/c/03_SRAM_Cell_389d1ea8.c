/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\03_Memory_Circuits\03_SRAM_Cell.md
 * Line: 1000
 * Language: c
 * Block ID: 389d1ea8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C语言代码与Cache SRAM的对应关系

// 示例1: 数组访问
int array[1024];  // 4KB数组，可能映射到多个Cache行

int sum = 0;
for (int i = 0; i < 1024; i++) {
    sum += array[i];  // 每次访问可能引起Cache命中或缺失
}

/*
 * Cache行为分析：
 * 
 * 假设：Cache行大小64字节，int=4字节
 * 
 * 数组占用: 1024 × 4B = 4KB
 * Cache行数: 4KB / 64B = 64行
 * 
 * 访问模式：
 * - 第0次访问 array[0]: 冷缺失 (Cold Miss)
 *   → 从内存加载64字节到Cache (array[0]-array[15])
 *   
 * - 第1-15次访问 array[1]-array[15]: 命中 (Hit)
 *   → 直接从Data SRAM读取
 *   
 * - 第16次访问 array[16]: 冷缺失
 *   → 从内存加载下一行
 *   
 * 总缺失数: 64次 (每行第一次访问)
 * 总命中数: 960次
 * 命中率: 960/1024 = 93.75%
 */

// 示例2: 结构体访问
struct Node {
    int key;        // 偏移0
    int value;      // 偏移4
    struct Node* next;  // 偏移8
};

struct Node* head = ...;
while (head != NULL) {
    printf("%d: %d\n", head->key, head->value);
    head = head->next;
}

/*
 * Cache行为分析：
 * 
 * 结构体大小: 12字节 (假设64位指针)
 * 每次访问结构体可能涉及：
 * 
 * 1. 访问 head->key:
 *    - 地址: head + 0
 *    - 可能命中或缺失
 * 
 * 2. 访问 head->value:
 *    - 地址: head + 4
 *    - 大概率命中（与key在同一Cache行）
 * 
 * 3. 访问 head->next:
 *    - 地址: head + 8
 *    - 大概率命中（与前两个字段在同一Cache行）
 * 
 * 空间局部性优化:
 * - 将小字段放在一起，减少Cache行访问
 * - 避免伪共享（False Sharing）
 */

// 示例3: 伪共享问题
struct {
    int counter1;   // CPU 0频繁修改
    int counter2;   // CPU 1频繁修改
} shared;  // 两个字段在同一Cache行！

/*
 * 问题：
 * - counter1和counter2在同一64字节Cache行
 * - CPU 0修改counter1 → 整个Cache行变为Modified
 * - CPU 1需要修改counter2 → 必须从CPU 0获取最新副本
 * - 频繁的Cache一致性流量，性能急剧下降
 * 
 * 解决方案：填充到不同Cache行
 */
struct {
    int counter1;
    char pad[60];   // 填充到64字节边界
    int counter2;
} separated;
