/*
 * Auto-generated from: 06_Thinking_Representation\03_Mind_Maps\03_Pointer_Concepts_Map.md
 * Line: 81
 * Language: c
 * Block ID: a7b967aa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

  int *p = malloc(sizeof(int));
  free(p);
  // p 现在是悬垂指针
  *p = 10;         // 危险！
  