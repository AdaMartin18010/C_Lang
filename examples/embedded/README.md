# 嵌入式编程示例

裸机/嵌入式 C 常用技术模拟。

## 文件说明

| 文件 | 说明 |
|------|------|
| `memory_mapped_io.c` | GPIO 寄存器内存映射访问，volatile 指针操作 |
| `bitfields_register.c` | union+struct 位域定义硬件寄存器 |
| `interrupt_simulation.c` | 中断向量表注册、使能、触发与分发 |

## 编译运行

```bash
make
./memory_mapped_io
./bitfields_register
./interrupt_simulation
```
