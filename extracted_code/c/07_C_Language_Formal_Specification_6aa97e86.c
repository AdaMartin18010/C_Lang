/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\07_C_Language_Formal_Specification.md
 * Line: 401
 * Language: c
 * Block ID: 6aa97e86
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

名等价 (C 规则):
  struct S { int x; } 和 struct T { int x; } 是不同的类型
  typedef 创建别名，与原类型等价

结构体自引用:
  struct Node { int data; struct Node *next; }  ✓
  struct Node { int data; struct Node next; }   ✗ (无限大小)
