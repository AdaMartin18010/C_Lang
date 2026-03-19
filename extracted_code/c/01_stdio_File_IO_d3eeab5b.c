/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\01_Standard_IO\01_stdio_File_IO.md
 * Line: 340
 * Language: c
 * Block ID: d3eeab5b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Windows上，文本模式会将\r\n转换为\n
// 错误：用文本模式读写二进制数据
FILE *fp = fopen("image.jpg", "r");  // 应该用"rb"!
