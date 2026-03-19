/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\16_MISRA_C_2023_Rules_16.md
 * Line: 300
 * Language: c
 * Block ID: 2ad263cb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 使用函数指针表 */
typedef int (*Operation)(int, int);

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }

static const Operation operations[] = {
    [0] = add,
    [1] = sub,
    [2] = mul
};

#define NUM_OPS (sizeof(operations) / sizeof(operations[0]))

int execute_op(int op_code, int a, int b, int *result)
{
    if (op_code < 0 || (size_t)op_code >= NUM_OPS) {
        return ERROR_INVALID_OPCODE;
    }
    
    if (operations[op_code] == NULL) {
        return ERROR_NOT_IMPLEMENTED;
    }
    
    *result = operations[op_code](a, b);
    return 0;
}
