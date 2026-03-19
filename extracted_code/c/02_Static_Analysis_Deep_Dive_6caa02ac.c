/*
 * Auto-generated from: 07_Modern_Toolchain\05_Code_Quality_Toolchain\02_Static_Analysis_Deep_Dive.md
 * Line: 268
 * Language: c
 * Block ID: 6caa02ac
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 源代码
int factorial(int n) {
    if (n <= 1)          // Node 1
        return 1;        // Node 2
    return n * factorial(n - 1);  // Node 3
}

// 控制流图
//
//      [Entry]
//         │
//         ▼
//    ┌─────────┐
//    │n <= 1?  │◄─────┐
//    └────┬────┘      │
//   Yes   │    No     │
//    │    ▼           │
//    │  [return       │
//    │   n * fact()]  │
//    │    │           │
//    ▼    ▼           │
//  [return 1]         │
//    │                │
//    └────┬───────────┘
//         ▼
//       [Exit]
