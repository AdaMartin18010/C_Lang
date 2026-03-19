/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\04_Control_Flow.md
 * Line: 753
 * Language: c
 * Block ID: 69b9089e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 悬空else问题
if (condition1)
    if (condition2)
        do_something();
else  // 这个else属于哪个if？
    do_other();

// ✅ 始终使用花括号
if (condition1) {
    if (condition2) {
        do_something();
    } else {
        do_other();
    }
}
