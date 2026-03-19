/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\01_Standard_IO\01_stdio_File_IO.md
 * Line: 348
 * Language: c
 * Block ID: 070e4df7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

char buffer[10];
fscanf(fp, "%s", buffer);  // 危险！可能溢出

// 正确做法
fscanf(fp, "%9s", buffer);  // 限制读取长度
