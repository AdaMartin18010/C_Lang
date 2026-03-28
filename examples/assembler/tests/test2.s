# test2.s - RISC-V 加载/存储测试
# ================================
# 测试I型加载指令和S型存储指令

.text
    # 初始化一些寄存器
    addi x5, zero, 0x100    # x5 = 基地址
    addi x6, zero, 0x123    # x6 = 测试数据

    # 存储指令测试 (S型)
    sw   x6, 0(x5)          # MEM[0x100] = 0x123
    sw   x6, 4(x5)          # MEM[0x104] = 0x123
    sh   x6, 8(x5)          # MEM[0x108](半字) = 0x23
    sb   x6, 12(x5)         # MEM[0x10C](字节) = 0x23

    # 加载指令测试 (I型)
    lw   x7, 0(x5)          # x7 = MEM[0x100]
    lw   x8, 4(x5)          # x8 = MEM[0x104]
    lh   x9, 8(x5)          # x9 = MEM[0x108] (符号扩展半字)
    lhu  x10, 8(x5)         # x10 = MEM[0x108] (零扩展半字)
    lb   x11, 12(x5)        # x11 = MEM[0x10C] (符号扩展字节)
    lbu  x12, 12(x5)        # x12 = MEM[0x10C] (零扩展字节)

    # 栈操作示例
    addi sp, sp, -16        # 分配栈帧
    sw   ra, 12(sp)         # 保存返回地址
    sw   s0, 8(sp)          # 保存保存寄存器
    sw   a0, 0(sp)          # 保存参数

    # 恢复
    lw   a0, 0(sp)
    lw   s0, 8(sp)
    lw   ra, 12(sp)
    addi sp, sp, 16         # 释放栈帧
