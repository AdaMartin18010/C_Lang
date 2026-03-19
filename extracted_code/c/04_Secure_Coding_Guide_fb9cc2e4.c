/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\04_Secure_Coding_Guide.md
 * Line: 182
 * Language: c
 * Block ID: fb9cc2e4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 漏洞代码：整数溢出导致缓冲区过小
void *vulnerable_alloc(size_t count, size_t size) {
    return malloc(count * size);  // ⚠️ 乘法溢出
}

// 攻击：count=0x40000000, size=4 → 溢出为0，分配0字节但后续写入16GB
