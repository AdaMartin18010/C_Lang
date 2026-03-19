/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\02_Data_Type_System.md
 * Line: 402
 * Language: c
 * Block ID: 37b5fd80
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// UNSAFE: 可能永远不成立
if (a + b == 0.3) {  // 危险！
    // ...
}

// UNSAFE: 累积误差
float sum = 0.0f;
for (int i = 0; i < 1000000; i++) {
    sum += 0.1f;  // 误差累积！
}
// sum 实际值约为 100958.34，而非 100000.0
