/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 336
 * Language: c
 * Block ID: 819c6d46
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int scores[100];

// C23之前的写法
for (int i = 0; i < sizeof(scores) / sizeof(scores[0]); i++) {
    scores[i] = 0;
}

// C23的现代化写法
for (int i = 0; i < _Countof(scores); i++) {
    scores[i] = 0;
}
