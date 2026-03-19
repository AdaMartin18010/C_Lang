/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\11_Lab11_Optional_Challenges.md
 * Line: 61
 * Language: c
 * Block ID: e0fbdff3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 当物理内存不足时:
// 1. 选择不活跃的页面
// 2. 压缩页面内容
// 3. 释放物理页
// 4. 需要时解压

struct compressed_page {
    uint32_t size;          // 压缩后大小
    uint8_t data[];         // 压缩数据
};

// 使用LZ4等快速压缩算法
