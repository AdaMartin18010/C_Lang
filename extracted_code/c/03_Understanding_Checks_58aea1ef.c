/*
 * Auto-generated from: 06_Thinking_Representation\09_Bloom_Taxonomy\03_Understanding_Checks.md
 * Line: 177
 * Language: c
 * Block ID: 58aea1ef
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

Q13: 解释"柔性数组成员(Flexible Array Member, FAM)"。
    要求: 说明使用FAM的结构体分配方法。

参考答案要点:
- C99特性: struct { int n; char data[]; };
- 特点: data不占用结构体大小(sizeof忽略它)
- 分配: malloc(sizeof(struct) + data_size)
- 优势: 一次分配，内存连续，缓存友好
- 替代: data[1]或data[0]的传统写法
- 注意: 只能作为结构体最后一个成员
