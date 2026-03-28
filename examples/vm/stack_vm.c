/*
 * 栈式虚拟机实现
 * ==============
 * 配套文档: knowledge/23_VM_Implementation/01_Stack_Based_VM.md
 *
 * 本虚拟机演示栈式计算模型，是理解VM翻译器和解释器的基础。
 * 支持基本算术、逻辑、内存访问、函数调用。
 *
 * 编译: gcc -Wall -Wextra -std=c99 -o stack_vm stack_vm.c
 * 运行: ./stack_vm test_program.vm
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#define STACK_SIZE 1024
#define HEAP_SIZE 1024
#define PROGRAM_MAX 65536

/* 指令操作码 */
typedef enum {
    OP_PUSH_CONST,      /* 将常量压栈 */
    OP_PUSH_LOCAL,      /* 将局部变量压栈 */
    OP_PUSH_ARG,        /* 将参数压栈 */
    OP_PUSH_STATIC,     /* 将静态变量压栈 */
    OP_POP_LOCAL,       /* 弹栈到局部变量 */
    OP_POP_ARG,         /* 弹栈到参数 */
    OP_POP_STATIC,      /* 弹栈到静态变量 */
    OP_POP_TEMP,        /* 弹栈到临时寄存器 */
    OP_ADD,             /* 加法 */
    OP_SUB,             /* 减法 */
    OP_NEG,             /* 取反 */
    OP_EQ,              /* 等于 */
    OP_GT,              /* 大于 */
    OP_LT,              /* 小于 */
    OP_AND,             /* 与 */
    OP_OR,              /* 或 */
    OP_NOT,             /* 非 */
    OP_JUMP,            /* 无条件跳转 */
    OP_JUMP_IF_FALSE,   /* 条件跳转 */
    OP_CALL,            /* 函数调用 */
    OP_RETURN,          /* 函数返回 */
    OP_FUNCTION,        /* 函数声明 */
    OP_LABEL,           /* 标签 */
    OP_HALT             /* 停机 */
} OpCode;

/* 指令结构 */
typedef struct {
    OpCode op;
    int arg1;           /* 第一个参数 */
    int arg2;           /* 第二个参数 */
    char label[64];     /* 标签名（用于跳转） */
} Instruction;

/* 虚拟机状态 */
typedef struct {
    int stack[STACK_SIZE];
    int heap[HEAP_SIZE];
    int local[256];     /* 局部变量段 */
    int argument[256];  /* 参数段 */
    int static_var[256]; /* 静态变量段 */
    int temp[8];        /* 临时寄存器 */
    
    int sp;             /* 栈指针 */
    int fp;             /* 帧指针 */
    int ip;             /* 指令指针 */
    int program_size;
    
    Instruction program[PROGRAM_MAX];
} VM;

/* 初始化虚拟机 */
void vm_init(VM *vm) {
    vm->sp = -1;
    vm->fp = 0;
    vm->ip = 0;
    vm->program_size = 0;
    memset(vm->stack, 0, sizeof(vm->stack));
    memset(vm->local, 0, sizeof(vm->local));
    memset(vm->argument, 0, sizeof(vm->argument));
    memset(vm->static_var, 0, sizeof(vm->static_var));
}

/* 压栈 */
void vm_push(VM *vm, int value) {
    if (vm->sp >= STACK_SIZE - 1) {
        fprintf(stderr, "Error: Stack overflow\n");
        exit(1);
    }
    vm->stack[++vm->sp] = value;
}

/* 弹栈 */
int vm_pop(VM *vm) {
    if (vm->sp < 0) {
        fprintf(stderr, "Error: Stack underflow\n");
        exit(1);
    }
    return vm->stack[vm->sp--];
}

/* 查看栈顶 */
int vm_peek(VM *vm) {
    if (vm->sp < 0) {
        fprintf(stderr, "Error: Stack empty\n");
        exit(1);
    }
    return vm->stack[vm->sp];
}

/* 查找标签地址 */
int find_label(VM *vm, const char *label) {
    for (int i = 0; i < vm->program_size; i++) {
        if (vm->program[i].op == OP_LABEL &&
            strcmp(vm->program[i].label, label) == 0) {
            return i;
        }
    }
    return -1;
}

/* 执行单条指令 */
void vm_execute_instruction(VM *vm) {
    if (vm->ip >= vm->program_size) {
        fprintf(stderr, "Error: Program counter out of bounds\n");
        exit(1);
    }
    
    Instruction *inst = &vm->program[vm->ip++];
    int a, b, result;
    int addr;
    
    switch (inst->op) {
        /* 数据操作 */
        case OP_PUSH_CONST:
            vm_push(vm, inst->arg1);
            break;
            
        case OP_PUSH_LOCAL:
            vm_push(vm, vm->local[vm->fp + inst->arg1]);
            break;
            
        case OP_PUSH_ARG:
            vm_push(vm, vm->argument[inst->arg1]);
            break;
            
        case OP_PUSH_STATIC:
            vm_push(vm, vm->static_var[inst->arg1]);
            break;
            
        case OP_POP_LOCAL:
            vm->local[vm->fp + inst->arg1] = vm_pop(vm);
            break;
            
        case OP_POP_ARG:
            vm->argument[inst->arg1] = vm_pop(vm);
            break;
            
        case OP_POP_STATIC:
            vm->static_var[inst->arg1] = vm_pop(vm);
            break;
            
        case OP_POP_TEMP:
            vm->temp[inst->arg1] = vm_pop(vm);
            break;
            
        /* 算术运算 */
        case OP_ADD:
            b = vm_pop(vm);
            a = vm_pop(vm);
            vm_push(vm, a + b);
            break;
            
        case OP_SUB:
            b = vm_pop(vm);
            a = vm_pop(vm);
            vm_push(vm, a - b);
            break;
            
        case OP_NEG:
            a = vm_pop(vm);
            vm_push(vm, -a);
            break;
            
        /* 比较运算 */
        case OP_EQ:
            b = vm_pop(vm);
            a = vm_pop(vm);
            vm_push(vm, a == b ? -1 : 0);  /* -1表示true，0表示false */
            break;
            
        case OP_GT:
            b = vm_pop(vm);
            a = vm_pop(vm);
            vm_push(vm, a > b ? -1 : 0);
            break;
            
        case OP_LT:
            b = vm_pop(vm);
            a = vm_pop(vm);
            vm_push(vm, a < b ? -1 : 0);
            break;
            
        /* 逻辑运算 */
        case OP_AND:
            b = vm_pop(vm);
            a = vm_pop(vm);
            vm_push(vm, a & b);
            break;
            
        case OP_OR:
            b = vm_pop(vm);
            a = vm_pop(vm);
            vm_push(vm, a | b);
            break;
            
        case OP_NOT:
            a = vm_pop(vm);
            vm_push(vm, ~a);
            break;
            
        /* 控制流 */
        case OP_JUMP:
            addr = find_label(vm, inst->label);
            if (addr < 0) {
                fprintf(stderr, "Error: Label not found: %s\n", inst->label);
                exit(1);
            }
            vm->ip = addr + 1;  /* 跳过标签定义 */
            break;
            
        case OP_JUMP_IF_FALSE:
            a = vm_pop(vm);
            if (a == 0) {
                addr = find_label(vm, inst->label);
                if (addr < 0) {
                    fprintf(stderr, "Error: Label not found: %s\n", inst->label);
                    exit(1);
                }
                vm->ip = addr + 1;
            }
            break;
            
        case OP_LABEL:
            /* 标签只是标记，无需执行 */
            break;
            
        /* 函数调用 */
        case OP_FUNCTION:
            /* 函数入口，设置局部变量空间 */
            /* arg1 = 局部变量数量 */
            break;
            
        case OP_CALL:
            /* 保存返回地址和帧指针 */
            /* 参数已在栈上 */
            /* 设置新的帧指针 */
            {
                int ret_addr = vm->ip;
                int num_args = inst->arg2;
                
                /* 保存返回地址和旧fp */
                vm_push(vm, ret_addr);
                vm_push(vm, vm->fp);
                
                /* 设置新fp */
                vm->fp = vm->sp - num_args - 1;
                
                /* 跳转到函数 */
                addr = find_label(vm, inst->label);
                if (addr < 0) {
                    fprintf(stderr, "Error: Function not found: %s\n", inst->label);
                    exit(1);
                }
                vm->ip = addr + 1;
            }
            break;
            
        case OP_RETURN:
            {
                /* 获取返回值 */
                result = vm_pop(vm);
                
                /* 恢复栈到调用前状态 */
                vm->sp = vm->fp - 1;
                
                /* 恢复fp和ip */
                int saved_fp = vm_pop(vm);
                int ret_addr = vm_pop(vm);
                vm->fp = saved_fp;
                vm->ip = ret_addr;
                
                /* 压入返回值 */
                vm_push(vm, result);
            }
            break;
            
        case OP_HALT:
            printf("\n[VM Halted]\n");
            return;
            
        default:
            fprintf(stderr, "Error: Unknown opcode %d\n", inst->op);
            exit(1);
    }
}

/* 运行虚拟机 */
void vm_run(VM *vm, int verbose) {
    if (verbose) {
        printf("Starting VM execution...\n");
        printf("Program size: %d instructions\n\n", vm->program_size);
    }
    
    while (vm->ip < vm->program_size) {
        if (verbose) {
            printf("IP=%04d SP=%04d FP=%04d ", vm->ip, vm->sp, vm->fp);
            if (vm->sp >= 0) {
                printf("Stack[top]=%d ", vm->stack[vm->sp]);
            }
            printf("\n");
        }
        
        vm_execute_instruction(vm);
    }
}

/* 解析指令 */
int parse_instruction(const char *line, Instruction *inst) {
    char op_str[64];
    char arg1_str[64] = "";
    char arg2_str[64] = "";
    
    /* 跳过空白 */
    while (*line && isspace((unsigned char)*line)) line++;
    if (*line == '\0' || *line == '/') return 0;  /* 空行或注释 */
    
    /* 解析操作码 */
    int n = sscanf(line, "%s %s %s", op_str, arg1_str, arg2_str);
    if (n < 1) return 0;
    
    /* 标签定义 */
    if (op_str[strlen(op_str)-1] == ':') {
        op_str[strlen(op_str)-1] = '\0';
        inst->op = OP_LABEL;
        strncpy(inst->label, op_str, 63);
        inst->label[63] = '\0';
        return 1;
    }
    
    /* 指令映射 */
    if (strcmp(op_str, "push_const") == 0) {
        inst->op = OP_PUSH_CONST;
        inst->arg1 = atoi(arg1_str);
    } else if (strcmp(op_str, "push_local") == 0) {
        inst->op = OP_PUSH_LOCAL;
        inst->arg1 = atoi(arg1_str);
    } else if (strcmp(op_str, "push_arg") == 0) {
        inst->op = OP_PUSH_ARG;
        inst->arg1 = atoi(arg1_str);
    } else if (strcmp(op_str, "push_static") == 0) {
        inst->op = OP_PUSH_STATIC;
        inst->arg1 = atoi(arg1_str);
    } else if (strcmp(op_str, "pop_local") == 0) {
        inst->op = OP_POP_LOCAL;
        inst->arg1 = atoi(arg1_str);
    } else if (strcmp(op_str, "pop_arg") == 0) {
        inst->op = OP_POP_ARG;
        inst->arg1 = atoi(arg1_str);
    } else if (strcmp(op_str, "pop_static") == 0) {
        inst->op = OP_POP_STATIC;
        inst->arg1 = atoi(arg1_str);
    } else if (strcmp(op_str, "pop_temp") == 0) {
        inst->op = OP_POP_TEMP;
        inst->arg1 = atoi(arg1_str);
    } else if (strcmp(op_str, "add") == 0) {
        inst->op = OP_ADD;
    } else if (strcmp(op_str, "sub") == 0) {
        inst->op = OP_SUB;
    } else if (strcmp(op_str, "neg") == 0) {
        inst->op = OP_NEG;
    } else if (strcmp(op_str, "eq") == 0) {
        inst->op = OP_EQ;
    } else if (strcmp(op_str, "gt") == 0) {
        inst->op = OP_GT;
    } else if (strcmp(op_str, "lt") == 0) {
        inst->op = OP_LT;
    } else if (strcmp(op_str, "and") == 0) {
        inst->op = OP_AND;
    } else if (strcmp(op_str, "or") == 0) {
        inst->op = OP_OR;
    } else if (strcmp(op_str, "not") == 0) {
        inst->op = OP_NOT;
    } else if (strcmp(op_str, "jump") == 0) {
        inst->op = OP_JUMP;
        strncpy(inst->label, arg1_str, 63);
        inst->label[63] = '\0';
    } else if (strcmp(op_str, "jump_if_false") == 0) {
        inst->op = OP_JUMP_IF_FALSE;
        strncpy(inst->label, arg1_str, 63);
        inst->label[63] = '\0';
    } else if (strcmp(op_str, "call") == 0) {
        inst->op = OP_CALL;
        strncpy(inst->label, arg1_str, 63);
        inst->label[63] = '\0';
        inst->arg2 = atoi(arg2_str);
    } else if (strcmp(op_str, "return") == 0) {
        inst->op = OP_RETURN;
    } else if (strcmp(op_str, "function") == 0) {
        inst->op = OP_FUNCTION;
        inst->arg1 = atoi(arg1_str);
    } else if (strcmp(op_str, "halt") == 0) {
        inst->op = OP_HALT;
    } else {
        fprintf(stderr, "Error: Unknown instruction: %s\n", op_str);
        return 0;
    }
    
    return 1;
}

/* 加载程序 */
void vm_load_program(VM *vm, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }
    
    char line[256];
    int line_num = 0;
    
    while (fgets(line, sizeof(line), file) && vm->program_size < PROGRAM_MAX) {
        line_num++;
        
        /* 移除行尾换行 */
        line[strcspn(line, "\n")] = '\0';
        
        if (parse_instruction(line, &vm->program[vm->program_size])) {
            vm->program_size++;
        }
    }
    
    fclose(file);
    printf("Loaded %d instructions from %s\n", vm->program_size, filename);
}

/* 测试程序：计算 1+2+...+10 */
const char *test_program = 
    "// Initialize sum = 0, i = 1\n"
    "push_const 0\n"
    "pop_static 0      // sum\n"
    "push_const 1\n"
    "pop_static 1      // i\n"
    "\n"
    "loop_start:\n"
    "// if i > 10, goto done\n"
    "push_static 1     // i\n"
    "push_const 10\n"
    "gt\n"
    "jump_if_false continue\n"
    "jump done\n"
    "\n"
    "continue:\n"
    "// sum = sum + i\n"
    "push_static 0     // sum\n"
    "push_static 1     // i\n"
    "add\n"
    "pop_static 0      // sum\n"
    "\n"
    "// i = i + 1\n"
    "push_static 1     // i\n"
    "push_const 1\n"
    "add\n"
    "pop_static 1      // i\n"
    "\n"
    "jump loop_start\n"
    "\n"
    "done:\n"
    "push_static 0     // push final sum\n"
    "halt\n";

void write_test_program(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error creating test file");
        exit(1);
    }
    fprintf(file, "%s", test_program);
    fclose(file);
    printf("Created test program: %s\n", filename);
}

int main(int argc, char *argv[]) {
    VM vm;
    vm_init(&vm);
    
    const char *program_file;
    
    if (argc < 2) {
        /* 创建并运行测试程序 */
        program_file = "test_sum.vm";
        write_test_program(program_file);
    } else {
        program_file = argv[1];
    }
    
    vm_load_program(&vm, program_file);
    
    int verbose = (argc >= 3 && strcmp(argv[2], "-v") == 0);
    
    printf("\n=== VM Execution ===\n\n");
    vm_run(&vm, verbose);
    
    /* 显示结果 */
    if (vm.sp >= 0) {
        printf("\nFinal stack top: %d\n", vm.stack[vm.sp]);
        printf("Expected: 55 (1+2+...+10)\n");
    }
    
    return 0;
}
