# 宏与元编程示例

C 预处理器高级技巧与 C11 `_Generic`。

## 文件说明

| 文件 | 说明 |
|------|------|
| `x_macros.c` | X-Macro 元编程，从单一数据定义生成枚举/数组/switch |
| `generic_selection.c` | C11 `_Generic` 实现类型安全的泛型 `max` 和 `print` |
| `stringify_concat.c` | `#` 字符串化、`##` 标记粘贴、`__attribute__((cleanup))` |

## 编译运行

```bash
make
./x_macros
./generic_selection
./stringify_concat
```
