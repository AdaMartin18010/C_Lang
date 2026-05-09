# C23 / C2y 示例代码索引

> **最后更新**: 2026-05-10

---

## C23 标准特性示例 (ISO/IEC 9899:2024)

| 编号 | 文件 | 特性 | 编译器要求 |
|------|------|------|-----------|
| 01 | `01_auto_type_inference.c` | `auto` 类型推导 | GCC 13+ / Clang 17+ |
| 02 | `02_typeof_operator.c` | `typeof` 操作符 | GCC 13+ / Clang 17+ |
| 03 | `03_nullptr_usage.c` | `nullptr` 空指针常量 | GCC 13+ / Clang 17+ |
| 04 | `04_constexpr_usage.c` | `constexpr` 编译期常量（对象级） | GCC 13+ / Clang 19+ |
| 05 | `05_attributes_syntax.c` | `[[...]]` 标准属性语法 | GCC 13+ / Clang 17+ |
| 06 | `06_bitint_arbitrary_precision.c` | `_BitInt(N)` 任意宽度整数 | GCC 13+ / Clang 15+ |
| 07 | `07_binary_constants.c` | `0b1010` 二进制常量 | GCC 13+ / Clang 9+ |
| 08 | `08_embed_directive.c` | `#embed` 嵌入二进制文件 | GCC 15+ / Clang 19+ |
| 09 | `09_static_assert.c` | 无消息 `static_assert` | GCC 13+ / Clang 17+ |
| 10 | `10_stdckdint_overflow_check.c` | `ckd_add` 溢出检测算术 | GCC 13+ / Clang 18+ |
| 11 | `11_new_math_functions.c` | C23数学库新函数 | GCC 13+ / Clang 17+ |
| 16 | `16_memset_explicit.c` | `memset_explicit` 防优化清零 | GCC 13+ / Clang 17+ |
| 17 | `17_strdup_strndup.c` | `strdup`/`strndup` 标准化 | GCC 13+ / Clang 17+ |
| 18 | `18_thread_safe_time.c` | `gmtime_r` 线程安全时间 | GCC 13+ / Clang 17+ |
| 19 | `19_digit_separators.c` | `1'000'000` 数字分隔符 | GCC 13+ / Clang 13+ |
| 20 | `20_compatibility_macros.c` | C23/C2y兼容宏 | GCC 15+ / Clang 20+ |

## C2y 草案特性示例 (ISO/IEC 9899:202Y)

| 编号 | 文件 | 特性 | 编译器要求 | 标准状态 |
|------|------|------|-----------|---------|
| 12 | `12_defer_statement.c` | `defer` 语句 | GCC 15+ (实验性) | TS CD r5 (N3853) |
| 13 | `13_countof_operator.c` | `_Countof` 操作符 | GCC 16+ | 推进中 |
| 14 | `14_if_declaration.c` | `if` 内声明 | GCC 15+ (实验性) | 已接受 |
| 15 | `15_named_loops.c` | 命名循环 | GCC 15+ / Clang 20+ | 已接受 |
| 21 | `21_elvis_operator.c` | Elvis 操作符 `?:` | GCC扩展 / Clang扩展 | 审议中 (N3753) |
| 22 | `22_counter_macro.c` | `__COUNTER__` 宏 | GCC扩展 / Clang扩展 | 审议中 (N3636) |
| 23 | `23_typeof_return.c` | `typeof(return)` | GCC扩展 / Clang扩展 | 审议中 (N3454) |

---

## 编译说明

### GCC

```bash
# C23 示例
gcc -std=c23 -Wall -Wextra -o example 01_auto_type_inference.c

# C2y 示例（需要 GCC 15+）
gcc -std=c2y -Wall -Wextra -o example 12_defer_statement.c
```

### Clang

```bash
# C23 示例
clang -std=c23 -Wall -Wextra -o example 01_auto_type_inference.c

# C2y 示例（需要 Clang 20+）
clang -std=c2y -Wall -Wextra -o example 15_named_loops.c
```

---

## 重要提示

- **MSVC 用户注意**: 截至2026年5月，MSVC 仍不支持 `_BitInt`、`nullptr`、`#embed` 等C23核心特性。跨平台项目建议以 GCC/Clang 为主编译器。
- **C23默认标准**: GCC 15+ 默认使用 `-std=gnu23`。编译旧代码时可能需要显式指定 `-std=gnu17`。
- **C2y实验性**: 所有C2y特性均为草案阶段，可能在最终标准中发生变化。
