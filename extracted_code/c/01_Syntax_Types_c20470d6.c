/*
 * Auto-generated from: 17_Ada_SPARK\01_Ada_Basics\01_Syntax_Types.md
 * Line: 180
 * Language: c
 * Block ID: c20470d6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

enum Color { Red, Green, Blue };
enum Color c = Red;

// C枚举本质上是整数
int x = Red;  // 允许！类型安全弱
c = 5;        // 允许！危险
