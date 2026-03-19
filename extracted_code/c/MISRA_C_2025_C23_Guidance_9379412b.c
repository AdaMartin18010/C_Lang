/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_C23_Guidance.md
 * Line: 423
 * Language: c
 * Block ID: 9379412b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 最佳实践 [待官方确认] */

/* 1. 文件大小检查 */
#if __STDC_VERSION__ >= 202311L
    static const uint8_t data[] = {
        #embed "resource.bin"
    };

    /* 编译时大小检查 */
    static_assert(sizeof(data) <= 65536, "Resource too large");
#endif

/* 2. 使用命名空间 */
static const uint8_t RESOURCE_LOGO_DATA[] = {
    #embed "logo.png"
};
static const size_t RESOURCE_LOGO_SIZE = sizeof(RESOURCE_LOGO_DATA);
