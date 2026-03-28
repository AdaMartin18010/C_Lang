# test1.s - RISC-V 基础算术测试
# ===============================
# 测试R型指令和I型算术指令

.text
    # R型指令测试
    add  x5, x6, x7         # x5 = x6 + x7
    sub  x8, x9, x10        # x8 = x9 - x10
    sll  x11, x12, x13      # x11 = x12 << x13
    slt  x14, x15, x16      # x14 = (x15 < x16) ? 1 : 0
    xor  x17, x18, x19      # x17 = x18 ^ x19
    srl  x20, x21, x22      # x20 = x21 >> x22 (逻辑右移)
    sra  x23, x24, x25      # x23 = x24 >> x25 (算术右移)
    or   x26, x27, x28      # x26 = x27 | x28
    and  x29, x30, x31      # x29 = x30 & x31

    # I型算术指令测试
    addi t0, zero, 100      # t0 = 100
    addi t1, t0, -50        # t1 = t0 - 50
    xori t2, t0, 0xFF       # t2 = t0 ^ 0xFF
    ori  t3, t0, 0x0F       # t3 = t0 | 0x0F
    andi t4, t0, 0xF0       # t4 = t0 & 0xF0
    slli t5, t0, 4          # t5 = t0 << 4
    srli t6, t0, 2          # t6 = t0 >> 2
    srai a0, t0, 3          # a0 = t0 >> 3 (算术)

    # 使用ABI名称
    addi sp, sp, -16        # 分配栈空间
    addi s0, sp, 0          # 保存帧指针
