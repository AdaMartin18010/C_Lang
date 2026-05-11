# 错误处理示例

C 语言错误处理惯用法的可编译示例。

## 文件说明

| 文件 | 说明 |
|------|------|
| `error_code_system.c` | 自定义错误码枚举 + 错误字符串 + 安全检查函数 |
| `goto_cleanup.c` | `goto cleanup` 统一释放多资源模式 |
| `raii_simulation.c` | GCC `__attribute__((cleanup))` 模拟 RAII |
| `setjmp_example.c` | `setjmp/longjmp` 模拟异常处理（TRY/CATCH/THROW） |

## 编译运行

```bash
make
./error_code_system
./goto_cleanup
./raii_simulation
./setjmp_example
```
