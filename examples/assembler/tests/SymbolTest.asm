// SymbolTest.asm - 符号表测试
// =============================
// 测试预定义符号、变量和标签

// 测试预定义符号
    @R0
    @R1
    @R5
    @R10
    @R15
    @SP
    @LCL
    @ARG
    @THIS
    @THAT
    @SCREEN
    @KBD

// 测试变量
    @i
    @j
    @k
    @sum
    @count
    @temp
    @i      // 重复使用，应该分配到同一地址

// 测试标签
(TEST_LABEL_1)
    0;JMP
(TEST_LABEL_2)
    @TEST_LABEL_1
    0;JMP
(TEST_LABEL_3)
    @TEST_LABEL_2
    D;JGT
