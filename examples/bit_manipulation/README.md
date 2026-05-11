# 位操作示例

可编译的 C 代码示例，演示位级操作技术。

## 文件说明

| 文件 | 说明 |
|------|------|
| `bitmap_allocator.c` | 64位位图分配器，使用 `__builtin_ctzll` 快速查找空闲位 |
| `bit_flags.c` | 标志位枚举与位操作宏（SET/CLEAR/TOGGLE/CHECK） |
| `endianness.c` | 字节序检测（编译期+运行期）与大小端转换函数 |

## 编译运行

```bash
make
./bitmap_allocator
./bit_flags
./endianness
```
