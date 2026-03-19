/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\02_CFI_Stack_Reconstruction.md
 * Line: 741
 * Language: c
 * Block ID: 14dad47f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 示例函数及其CFI（简化版）
/*
void example_func(int x, int y) {
    int local = x + y;
    callee(local);
}

汇编（x86-64 System V ABI）:
    pushq   %rbp            # CFI: 保存旧帧指针
    .cfi_def_cfa_offset 16   # CFA = RSP + 16
    .cfi_offset 6, -16       # RBP保存在CFA-16

    movq    %rsp, %rbp      # 设置新帧指针
    .cfi_def_cfa_register 6  # CFA = RBP + 16

    subq    $16, %rsp       # 分配栈空间
    ...

    leave                   # 恢复RSP和RBP
    .cfi_def_cfa 7, 8        # CFA = RSP + 8
    ret
    .cfi_endproc
*/

// 对应的CFI字节码解析示例
void analyze_example_cfi(void) {
    // CIE（共享）
    uint8_t cie_bytes[] = {
        // CIE头
        0x14, 0x00, 0x00, 0x00,     // 长度 = 20
        0xFF, 0xFF, 0xFF, 0xFF,     // CIE ID = -1
        0x01,                       // 版本 = 1
        'z', 'R', 0,                // 扩展字符串
        0x01,                       // code_align = 1
        0x78,                       // data_align = -8 (SLEB128)
        0x10,                       // return_addr_reg = 16 (RIP)
        0x01, 0x1B,                 // 扩展：长度=1, FDE编码=DW_EH_PE_pcrel|DW_EH_PE_sdata4
        // 初始指令
        0x0C, 0x07, 0x08,           // def_cfa rsp, 8
        0x80 | 0x10, 0x00,          // offset rip, 0 (SAME_VAL)
        0x00                        // nop
    };

    // FDE
    uint8_t fde_bytes[] = {
        // FDE头
        0x24, 0x00, 0x00, 0x00,     // 长度 = 36
        0x1C, 0x00, 0x00, 0x00,     // CIE指针（相对偏移）
        // 位置信息
        0x00, 0x10, 0x00, 0x00,     // 函数起始（PC相对）
        0x00, 0x00, 0x00, 0x00,     // 函数长度
        // 指令
        0x41,                       // advance_loc 1（push rbp后）
        0x0E, 0x10,                 // def_cfa_offset 16
        0x86, 0x02,                 // offset rbp, -16
        0x41,                       // advance_loc 1（mov rbp,rsp后）
        0x0D, 0x06,                 // def_cfa_register rbp
        // ... 更多指令
        0x00                        // nop
    };

    // 解析验证
    printf("示例CFI分析:\n");
    printf("1. 函数入口: CFA = RSP + 8, RIP在CFA-8\n");
    printf("2. push %rbp后: CFA = RSP + 16, RBP保存在CFA-16\n");
    printf("3. mov %rbp,%rsp后: CFA = RBP + 16\n");
    printf("4. 函数退出: 恢复初始CFA定义\n");
}
