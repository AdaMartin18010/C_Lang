/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\02_CFI_Stack_Reconstruction.md
 * Line: 84
 * Language: c
 * Block ID: 17217b0e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Common Information Entry (CIE)
// 包含共享的展开信息
typedef struct {
    uint64_t offset;                // 在.debug_frame中的偏移
    uint64_t length;
    uint64_t CIE_id;                // CIE ID（通常是0或-1）
    uint8_t version;
    char *augmentation;             // 扩展字符串
    uint64_t code_alignment_factor; // 代码对齐因子
    int64_t data_alignment_factor;  // 数据对齐因子
    uint64_t return_address_register; // 返回地址寄存器编号

    // 初始指令（对所有FDE共享）
    uint8_t *initial_instructions;
    size_t initial_instructions_length;

    // 解析后的寄存器规则
    RegisterRule initial_rules[REG_COUNT];
} CIE;

// Frame Description Entry (FDE)
// 描述特定函数的展开信息
typedef struct {
    uint64_t offset;                // 在.debug_frame中的偏移
    uint64_t length;
    uint64_t CIE_pointer;           // 指向关联CIE的偏移
    uint64_t initial_location;      // 函数起始地址
    uint64_t address_range;         // 函数地址范围

    // 程序指令（指令偏移→CFI指令）
    uint8_t *instructions;
    size_t instructions_length;

    // 指向关联的CIE
    CIE *cie;
} FDE;

// 寄存器规则类型
typedef enum {
    RULE_UNDEFINED,     // 未定义（调用者不关心）
    RULE_SAME_VALUE,    // 与调用时相同
    RULE_OFFSET,        // CFA + offset
    RULE_VAL_OFFSET,    // CFA + offset（值本身，非地址）
    RULE_REGISTER,      // 从其他寄存器复制
    RULE_EXPRESSION,    // DWARF表达式计算地址
    RULE_VAL_EXPRESSION,// DWARF表达式计算值
} RuleType;

// 寄存器规则
typedef struct {
    RuleType type;
    union {
        int64_t offset;             // 对于OFFSET/VAL_OFFSET
        uint64_t reg_num;           // 对于REGISTER
        struct {
            uint8_t *expr;
            size_t len;
        } expression;               // 对于EXPRESSION
    };
} RegisterRule;

// CFA（Canonical Frame Address）定义
typedef struct {
    enum { CFA_REGISTER, CFA_EXPRESSION } type;
    union {
        struct {
            uint64_t reg_num;       // 基址寄存器
            int64_t offset;         // 偏移
        } reg;
        struct {
            uint8_t *expr;
            size_t len;
        } expr;
    };
} CFADefinition;

// 完整展开上下文
typedef struct {
    uint64_t pc;                    // 当前程序计数器
    uint64_t cfa;                   // 规范帧地址
    RegisterRule rules[REG_COUNT];  // 各寄存器规则
    uint64_t registers[REG_COUNT];  // 寄存器值（执行时填充）
} UnwindContext;
