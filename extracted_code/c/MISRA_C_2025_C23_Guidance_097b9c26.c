/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_C23_Guidance.md
 * Line: 449
 * Language: c
 * Block ID: 097b9c26
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#if __STDC_VERSION__ >= 202311L
    /* [待官方确认] - 任意位宽整数 */

    /* ✅ 使用场景 - 精确位宽需求 */
    _BitInt(17) sensor_id;      /* 17位传感器ID */
    _BitInt(24) color_value;    /* 24位颜色值 */
    _BitInt(48) mac_address;    /* 48位MAC地址 */

    /* ⚠️ 避免 - 使用标准类型更清晰 */
    /* _BitInt(8) byte; */      /* 使用uint8_t */
    /* _BitInt(32) word; */     /* 使用uint32_t */

    /* ✅ 运算 */
    _BitInt(17) a = 100;
    _BitInt(17) b = 200;
    _BitInt(18) sum = a + b;    /* 结果可能需要更多位 */

#else
    /* C11/C17 - 使用位域或标准类型 */
    struct SensorID {
        uint32_t id : 17;
    };
    uint32_t sensor_id;  /* 浪费空间但兼容 */
#endif
