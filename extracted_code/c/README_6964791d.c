/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\12_Self_Modifying_Code\README.md
 * Line: 313
 * Language: c
 * Block ID: 6964791d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 热补丁技术：运行时替换函数实现

// 原始函数
int original_func(int x) {
    return x * 2;
}

// 新实现
int new_func(int x) {
    return x * 3;
}

// x86-64热补丁：5字节jmp相对跳转
void hot_patch(void *target, void *replacement) {
    // 计算相对偏移
    intptr_t offset = (intptr_t)replacement -
                      ((intptr_t)target + 5);

    // 修改内存保护为可写
    mprotect(PAGE_ALIGN(target), 4096,
             PROT_READ | PROT_WRITE | PROT_EXEC);

    // 写入跳转指令: E9 xx xx xx xx (jmp rel32)
    uint8_t *code = target;
    code[0] = 0xE9;  // jmp opcode
    memcpy(&code[1], &offset, 4);

    // 刷新缓存
    __builtin___clear_cache(target, (char*)target + 5);
}

// 使用
// hot_patch(original_func, new_func);
// original_func(10) 现在返回 30 而不是 20
