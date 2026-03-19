/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\02_Speculative_Execution.md
 * Line: 142
 * Language: c
 * Block ID: 62fe489c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Spectre V1 漏洞代码模式
// 攻击者控制index，可越界读取敏感数据

uint8_t secret_data[16];      // 敏感数据
uint8_t public_data[256 * 4096];  // 探针数组（大页）

// 易受攻击的代码
void vulnerable_function(size_t index, uint8_t *data) {
    // 边界检查
    if (index < array_size) {
        // 推测执行：即使index越界，也可能执行
        uint8_t value = data[index];  // 越界读取

        // 访问模式依赖于越界读取的值
        // 导致secret_data被加载到缓存
        temp &= public_data[value * 4096];
    }
}

// 攻击者通过缓存计时推断secret_data的值
