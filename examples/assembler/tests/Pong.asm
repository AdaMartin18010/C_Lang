// Pong.asm - 简化版乒乓球游戏
// ==============================
// 测试复杂的控制流和内存操作
// 这是Hack平台上的经典程序

    @256
    D=A
    @SP
    M=D         // 初始化栈指针

    @0
    D=A
    @LCL
    M=D         // 初始化局部段

    @300
    D=A
    @ARG
    M=D         // 初始化参数段

    @400
    D=A
    @THIS
    M=D         // 初始化this段

    @500
    D=A
    @THAT
    M=D         // 初始化that段

    @SCREEN
    D=A
    @screen
    M=D         // 保存屏幕基地址

    @KBD
    D=A
    @keyboard
    M=D         // 保存键盘地址

// 游戏主循环
(GAME_LOOP)
    // 检查键盘输入
    @keyboard
    A=M
    D=M

    @NO_KEY
    D;JEQ       // 如果没有按键，跳转

    @KEY_PRESSED
    0;JMP

(NO_KEY)
    // 没有按键时的处理
    @ball_x
    M=M+1       // 球向右移动
    @DRAW_BALL
    0;JMP

(KEY_PRESSED)
    @131        // 上箭头键
    D=D-A
    @MOVE_UP
    D;JEQ

    @133        // 下箭头键
    D=D-A
    @MOVE_DOWN
    D;JEQ

    @GAME_LOOP
    0;JMP

(MOVE_UP)
    @paddle_y
    M=M-1
    @DRAW_PADDLE
    0;JMP

(MOVE_DOWN)
    @paddle_y
    M=M+1
    @DRAW_PADDLE
    0;JMP

(DRAW_BALL)
    // 简化：在ball_x位置绘制点
    @ball_x
    D=M
    @screen
    D=D+M
    A=D
    M=-1
    @GAME_LOOP
    0;JMP

(DRAW_PADDLE)
    // 简化：绘制挡板
    @paddle_y
    D=M
    @screen
    D=D+M
    A=D
    M=-1
    A=A+1
    M=-1
    A=A+1
    M=-1
    @GAME_LOOP
    0;JMP

// 变量
(screen)
    @0
(keyboard)
    @0
(ball_x)
    @0
(ball_y)
    @0
(paddle_y)
    @0
