/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\10_Lab10_mmap.md
 * Line: 231
 * Language: c
 * Block ID: d5271033
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// mmap不立即分配物理页
// 而是记录映射信息到VMA
// 实际分配在页面错误时进行 (惰性分配)

// 用户空间看到的页表:
// 虚拟地址已分配，但PTE_V=0
// 首次访问触发页面错误
// 页面错误处理程序:
//   1. 查找对应VMA
//   2. 分配物理页
//   3. 如果是文件映射，读取文件内容
//   4. 建立映射
