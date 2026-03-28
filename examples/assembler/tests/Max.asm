// Max.asm - 求两个数中的最大值
// ==============================
// 测试条件跳转和基本ALU操作

    @R0     // D = RAM[0]
    D=M

    @R1     // D = RAM[0] - RAM[1]
    D=D-M

    @R0_IS_MAX
    D;JGT   // 如果R0 > R1，跳转到R0_IS_MAX

    // R1 >= R0
    @R1
    D=M
    @R2
    M=D     // RAM[2] = R1
    @END
    0;JMP

(R0_IS_MAX)
    @R0
    D=M
    @R2
    M=D     // RAM[2] = R0

(END)
    @END
    0;JMP   // 停机
