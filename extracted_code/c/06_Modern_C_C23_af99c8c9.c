/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\06_Modern_C_C23.md
 * Line: 396
 * Language: c
 * Block ID: af99c8c9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

switch (value) {
    case 1:
        do_something();
        [[fallthrough]];  // 明确表示有意穿透
    case 2:
        do_more();
        break;
    case 3:
        [[unlikely]] handle_rare();  // 分支预测提示
        break;
    default:
        handle_default();
        [[unlikely]] break;
}
