/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\04_Linux_Kernel_Toolchain.md
 * Line: 816
 * Language: c
 * Block ID: 8b55fc14
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 严格的编码规范
// - 缩进: Tab (8 字符)
// - 行长度: 80 字符
// - 括号: K&R 风格
// - 命名: 小写 + 下划线

int my_function(int arg1, int arg2)
{
    int result;

    if (arg1 < 0)
        return -EINVAL;

    result = arg1 + arg2;

    return result;
}

// 检查工具
// - checkpatch.pl - 补丁检查
// - sparse - 静态分析
// - coccinelle - 语义补丁
