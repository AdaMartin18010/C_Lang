/*
 * Auto-generated from: 16_Embedded_AI_Agents\08_Real_Time_AI\01_Deterministic_Inference.md
 * Line: 343
 * Language: c
 * Block ID: d803048c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ARM Cortex-M7 支持指令缓存锁定

// 将关键函数锁定到缓存
void lock_critical_function_to_cache(void) {
    extern uint32_t critical_function_start;
    extern uint32_t critical_function_end;

    // 逐出缓存行
    SCB_InvalidateICache();

    // 执行函数以加载到缓存
    critical_function();

    // 锁定缓存 (特定于MCU实现)
    // 需要参考具体芯片手册
}

// 数据缓存锁定
void lock_data_to_cache(void* data, size_t size) {
    uint32_t* ptr = (uint32_t*)data;
    uint32_t* end = (uint32_t*)((uint8_t*)data + size);

    // 访问每个缓存行以加载到缓存
    while (ptr < end) {
        volatile uint32_t dummy = *ptr;  // 强制加载
        ptr += 8;  // 32字节缓存行
    }

    // 锁定 (实现依赖于具体芯片)
}
