/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\03_Activation_Record.md
 * Line: 496
 * Language: c
 * Block ID: 94d76543
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 危险：未检查输入长度
void dangerous(char *input) {
    char buffer[64];
    strcpy(buffer, input);  // 危险！可能溢出覆盖返回地址
}

// 安全：使用边界检查版本
void safe(char *input) {
    char buffer[64];
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
}
