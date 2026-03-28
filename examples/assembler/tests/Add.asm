// Add.asm - 计算 1+2+...+100
// ===========================
// 这是经典的Hack汇编示例程序
// 用于测试A指令、C指令、标签和变量

    @i      // i = 1
    M=1

    @sum    // sum = 0
    M=0

(LOOP)      // 循环开始
    @i
    D=M     // D = i

    @100
    D=D-A   // D = i - 100

    @END
    D;JGT   // 如果i>100，跳转到END

    @i
    D=M     // D = i

    @sum
    M=D+M   // sum = sum + i

    @i
    M=M+1   // i = i + 1

    @LOOP
    0;JMP   // 跳转到LOOP

(END)
    @END
    0;JMP   // 无限循环（停机）
