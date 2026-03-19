/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\04_Control_Flow.md
 * Line: 206
 * Language: c
 * Block ID: bc3c1339
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C17 属性标记有意fall-through
switch (state) {
    case START:
        init();
        [[fallthrough]];  // C17/C23
    case RUNNING:
        process();
        break;
}

// C11/C17 宏方案
#ifndef __has_c_attribute
    #define __has_c_attribute(x) 0
#endif
#if __has_c_attribute(fallthrough)
    #define FALLTHROUGH [[fallthrough]]
#else
    #define FALLTHROUGH /* fall through */
#endif
