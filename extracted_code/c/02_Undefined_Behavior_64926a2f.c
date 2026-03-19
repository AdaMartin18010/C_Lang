/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\02_Undefined_Behavior.md
 * Line: 85
 * Language: c
 * Block ID: 64926a2f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 移位数量大于等于类型宽度
int x = 1 << 33;  // UB! (假设32位int)

// ❌ 左移导致符号位改变
int y = 0x40000000 << 1;  // UB! (符号位从0变1)

// ❌ 负数右移
int z = -1 >> 1;  // 实现定义

// ✅ 安全移位
unsigned int safe1 = 1U << 31;  // OK
unsigned int safe2 = 0x40000000U << 1;  // OK（无符号）
