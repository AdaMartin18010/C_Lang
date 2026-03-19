/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 32
 * Language: c
 * Block ID: e4210dc9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C2y defer 基本语法
FILE *file = fopen("data.txt", "r");
defer fclose(file);  // 这行代码会在函数退出时自动执行

// 继续正常的业务逻辑，无需关心文件关闭
process_file(file);
