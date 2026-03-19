/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\01_Bytecode_VM.md
 * Line: 73
 * Language: c
 * Block ID: bba7237e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单栈式虚拟机架构

typedef enum {
    OP_NOP,      // 空操作
    OP_PUSH,     // 压栈
    OP_POP,      // 出栈
    OP_ADD,      // 加法
    OP_SUB,      // 减法
    OP_MUL,      // 乘法
    OP_DIV,      // 除法
    OP_EQ,       // 等于
    OP_LT,       // 小于
    OP_JMP,      // 跳转
    OP_JZ,       // 条件跳转
    OP_CALL,     // 函数调用
    OP_RET,      // 返回
    OP_LOAD,     // 加载局部变量
    OP_STORE,    // 存储局部变量
    OP_CONST,    // 加载常量
    OP_PRINT,    // 打印
    OP_HALT,     // 停止
} OpCode;

// 指令格式
// 单字节操作码 + 操作数（变长）

// 虚拟机状态
typedef struct {
    // 程序
    uint8_t *code;      // 字节码
    size_t code_size;   // 代码大小

    // 常量池
    int64_t *constants; // 常量数组
    size_t const_count;

    // 执行状态
    int64_t *stack;     // 操作数栈
    size_t stack_size;  // 栈容量
    size_t sp;          // 栈指针

    size_t pc;          // 程序计数器
    int64_t *locals;    // 局部变量
    size_t local_count;

    // 调用帧
    typedef struct Frame {
        size_t return_addr;
        int64_t *saved_locals;
        size_t saved_sp;
    } Frame;

    Frame *call_stack;
    size_t fp;          // 帧指针
} VM;

// 创建虚拟机
VM *vm_create(size_t stack_size, size_t local_count) {
    VM *vm = calloc(1, sizeof(VM));
    vm->stack = calloc(stack_size, sizeof(int64_t));
    vm->stack_size = stack_size;
    vm->locals = calloc(local_count, sizeof(int64_t));
    vm->local_count = local_count;
    vm->call_stack = calloc(256, sizeof(Frame));
    return vm;
}
