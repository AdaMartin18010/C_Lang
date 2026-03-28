# test3.s - RISC-V 分支和跳转测试
# =================================
# 测试B型分支指令和J型跳转指令

.text
main:
    addi t0, zero, 10       # t0 = 10
    addi t1, zero, 20       # t1 = 20

    # 分支测试
    beq  t0, t1, not_equal  # 如果 t0 == t1 (false)
    bne  t0, t1, test_blt   # 如果 t0 != t1 (true)

not_equal:
    addi t2, zero, 1        # 不会执行到这里

test_blt:
    blt  t0, t1, test_bge   # 如果 t0 < t1 (true)
    addi t2, zero, 2        # 跳过

test_bge:
    bge  t1, t0, test_beq   # 如果 t1 >= t0 (true)
    addi t2, zero, 3        # 跳过

test_beq:
    beq  t0, t0, test_loop  # 如果 t0 == t0 (true)

# 简单循环测试
test_loop:
    addi t3, zero, 0        # i = 0
    addi t4, zero, 5        # limit = 5

loop_start:
    beq  t3, t4, loop_end   # 如果 i == limit, 退出
    addi t3, t3, 1          # i++
    j    loop_start         # 继续循环

loop_end:
    # 跳转指令测试
    jal  ra, sub_routine    # 调用子程序，返回地址存入ra
    jal  x0, continue       # 无条件跳转（不保存返回地址）

sub_routine:
    addi t5, zero, 42       # 返回值
    jalr x0, ra, 0          # 返回 (jr ra)

continue:
    # 条件选择
    addi a0, zero, 10
    addi a1, zero, 20
    blt  a0, a1, a0_is_less
    addi a2, zero, 0
    j    done

a0_is_less:
    addi a2, zero, 1

done:
    j    done               # 无限循环（停机）
