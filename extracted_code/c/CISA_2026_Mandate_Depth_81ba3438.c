/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\CISA_2026_Mandate_Depth.md
 * Line: 1968
 * Language: c
 * Block ID: 81ba3438
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 配置PMP区域
void setup_pmp() {
    // 区域0：只读代码区
    pmp_set_region(0, PMP_NAPOT | PMP_X, CODE_START, CODE_SIZE);

    // 区域1：读写数据区
    pmp_set_region(1, PMP_NAPOT | PMP_R | PMP_W, DATA_START, DATA_SIZE);

    // 区域2：无权限（捕获空指针）
    pmp_set_region(2, PMP_NA, 0, 0x1000);
}
