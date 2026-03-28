# test5.s - RISC-V 函数调用示例
# ==============================
# 测试函数调用约定和栈帧管理

.text
.globl main

# 主函数
main:
    # 设置参数
    li   a0, 5              # 第一个参数 n = 5
    jal  ra, factorial      # 调用 factorial(5)
    
    # 结果在a0中
    mv   s0, a0             # 保存结果
    
    # 测试另一个函数
    li   a0, 1
    li   a1, 2
    li   a2, 3
    jal  ra, sum3           # 调用 sum3(1, 2, 3)
    
    # 停机
end:
    j    end

# 计算阶乘的函数
# 输入: a0 = n
# 输出: a0 = n!
# 使用: t0 (临时), 保存s1
factorial:
    # 保存返回地址和s1
    addi sp, sp, -8
    sw   ra, 4(sp)
    sw   s1, 0(sp)
    
    mv   s1, a0             # s1 = n (保存参数)
    
    # 基准情况: if (n <= 1) return 1
    addi t0, zero, 1
    ble  s1, t0, fact_base
    
    # 递归: n * factorial(n-1)
    addi a0, s1, -1         # a0 = n - 1
    jal  ra, factorial      # 递归调用
    
    # 返回值在a0中
    mul  a0, s1, a0         # a0 = n * factorial(n-1)
    j    fact_end

fact_base:
    li   a0, 1

fact_end:
    # 恢复
    lw   s1, 0(sp)
    lw   ra, 4(sp)
    addi sp, sp, 8
    ret                     # 伪指令: jalr x0, ra, 0

# 计算三个数之和
# 输入: a0, a1, a2
# 输出: a0 = a0 + a1 + a2
sum3:
    add  a0, a0, a1
    add  a0, a0, a2
    ret

# 测试叶子函数（不调用其他函数）
# 不需要保存ra
square:
    mul  a0, a0, a0
    ret
