/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\04_Linux_Kernel_Toolchain.md
 * Line: 423
 * Language: c
 * Block ID: 62db9423
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 导出符号供其他模块使用 */

/* 导出给 GPL 模块 */
EXPORT_SYMBOL(my_function);
EXPORT_SYMBOL_GPL(my_gpl_function);

/* 导出变量 */
EXPORT_SYMBOL(my_variable);

/* GPL 符号 - 仅 GPL 模块可用 */
EXPORT_SYMBOL_GPL(my_internal_function);

/* 示例 */
int my_shared_function(int x)
{
    return x * 2;
}
EXPORT_SYMBOL(my_shared_function);

/* 符号版本控制（用于模块兼容性） */
EXPORT_SYMBOL_NS(my_function, MY_NAMESPACE);
