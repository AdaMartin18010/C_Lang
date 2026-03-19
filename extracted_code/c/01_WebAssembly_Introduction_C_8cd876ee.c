/*
 * Auto-generated from: 10_WebAssembly_C\01_WebAssembly_Introduction_C.md
 * Line: 221
 * Language: c
 * Block ID: 8cd876ee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码示例
int add(int a, int b) {
    return a + b;
}

// 对应的 WebAssembly 文本格式 (WAT)
(module
  (func $add (param i32 i32) (result i32)
    local.get 0    // 将第一个参数压入栈
    local.get 1    // 将第二个参数压入栈
    i32.add        // 弹出两个值，相加，压入结果
  )
  (export "add" (func $add))
)

// 执行栈状态变化
// 步骤1: local.get 0    → 栈: [a]
// 步骤2: local.get 1    → 栈: [a, b]
// 步骤3: i32.add        → 栈: [a+b]
