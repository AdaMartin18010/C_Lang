# 虚拟机与解释器

> **层级定位**: 03_System_Technology_Domains > 01_Virtual_Machine_Interpreter

---

## 内容

- [字节码虚拟机](./01_Bytecode_VM.md)
- [寄存器虚拟机](./02_Register_VM.md)
- [JIT编译](./03_JIT_Compilation.md)

---

## 虚拟机类型

| 类型 | 特点 | 示例 |
|:-----|:-----|:-----|
| 栈式 | 操作在栈上进行 | JVM、Python |
| 寄存器 | 使用寄存器操作 | Lua VM、Dalvik |
| 直接线程 | 高效指令分发 | Forth |

---

## C语言实现

```c
typedef enum {
    OP_PUSH,
    OP_POP,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_HALT
} OpCode;

typedef struct {
    int *code;
    int *stack;
    int pc;
    int sp;
} VM;

void vm_run(VM *vm);
```

---

> **最后更新**: 2026-03-21
