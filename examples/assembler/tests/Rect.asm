// Rect.asm - 在屏幕上绘制矩形
// =============================
// 测试内存映射I/O和循环结构

    @0
    D=M
    @INFINITE_LOOP
    D;JLE   // 如果RAM[0] <= 0，跳转到INFINITE_LOOP

    @counter
    M=D     // counter = RAM[0]（矩形高度）

    @SCREEN
    D=A
    @address
    M=D     // address = SCREEN基地址

(LOOP)
    @counter
    D=M
    @INFINITE_LOOP
    D;JLE   // 如果counter <= 0，结束

    // 绘制一行（16像素 = 1个word）
    @address
    A=M
    M=-1    // RAM[address] = 0xFFFF（所有像素点亮）

    @address
    M=M+1   // address++

    @counter
    M=M-1   // counter--

    @LOOP
    0;JMP

(INFINITE_LOOP)
    @INFINITE_LOOP
    0;JMP
