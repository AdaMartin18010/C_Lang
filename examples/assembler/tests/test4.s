# test4.s - RISC-V 伪指令测试
# ============================
# 测试各种伪指令的展开

.text
    # nop 伪指令
    nop                     # 展开为: addi x0, x0, 0

    # mv 伪指令
    addi x5, zero, 100
    mv   x6, x5             # 展开为: addi x6, x5, 0

    # li 伪指令 - 小立即数
    li   x7, 100            # 展开为: addi x7, x0, 100

    # li 伪指令 - 大立即数 (需要两条指令)
    li   x8, 0x12345        # 展开为: lui + addi

    # j 伪指令
    j    skip               # 展开为: jal x0, skip

    addi x9, zero, 999      # 被跳过

skip:
    # 更多li测试
    li   x10, -1            # 负数
    li   x11, 0             # 零
    li   x12, 2047          # 12位边界
    li   x13, -2048         # 12位边界（负数）
    li   x14, 2048          # 需要lui+addi
    li   x15, 0xABCDEF      # 大立即数

    # 组合测试
    li   t0, 10
    li   t1, 20
    add  t2, t0, t1
    mv   a0, t2
    nop
