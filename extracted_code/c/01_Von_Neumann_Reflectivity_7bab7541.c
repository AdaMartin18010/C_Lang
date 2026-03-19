/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\13_Self_Modifying_Code\01_Von_Neumann_Reflectivity.md
 * Line: 643
 * Language: c
 * Block ID: 7bab7541
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 自修改代码通常需要位置无关
// 错误：使用绝对地址
void wrong_pic(void) {
    uint8_t code[] = {
        0x48, 0xC7, 0xC0,
        0x00, 0x00, 0x00, 0x00,  // mov rax, <absolute addr>
        // ❌ 如果代码移动到不同地址，此引用失效
    };
}

// 正确：使用RIP相对寻址或自定位
void correct_pic(void) {
    // 方法1：RIP相对寻址（x86-64）
    uint8_t code1[] = {
        0x48, 0x8D, 0x05, 0x00, 0x00, 0x00, 0x00,  // lea rax, [rip]
    };

    // 方法2：调用/弹出获取当前地址（x86）
    uint8_t code2[] = {
        0xE8, 0x00, 0x00, 0x00, 0x00,  // call next
        0x58,                           // pop eax (获得地址)
    };
}
