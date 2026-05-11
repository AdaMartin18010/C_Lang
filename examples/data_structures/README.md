# 数据结构示例

纯 C 实现的经典数据结构，无外部依赖。

## 文件说明

| 文件 | 说明 |
|------|------|
| `intrusive_list.c` | Linux Kernel 风格侵入式链表，`container_of` 宏 |
| `list_algorithms.c` | 单链表算法：反转、Floyd判环、找中点、合并有序链表 |
| `hash_table.c` | 开放寻址法哈希表，FNV-1a 哈希函数 |
| `dynamic_array.c` | 泛型动态数组宏（类似 C++ vector） |
| `binary_tree.c` | 二叉搜索树：插入、查找、中序遍历、删除 |

## 编译运行

```bash
make
./intrusive_list
./list_algorithms
./hash_table
./dynamic_array
./binary_tree
```
