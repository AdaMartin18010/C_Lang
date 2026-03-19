/*
 * Auto-generated from: 06_Thinking_Representation\10_Concept_Maps\10_Error_Handling_Concept_Map.md
 * Line: 300
 * Language: c
 * Block ID: 0df55a0e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

[错误信息结构]
    │
    ├──[错误码]──► [机器可识别的错误标识]
    │
    ├──[错误信息]──► [人类可读的描述]
    │
    ├──[错误位置]──► [文件名、行号]
    │
    └──[错误链]──► [导致此错误的根本原因]

[实现示例]
    typedef struct Error {
        int code;
        const char *message;
        const char *file;
        int line;
        struct Error *cause;
    } Error;

    #define ERROR(code, msg) \
        (&(Error){(code), (msg), __FILE__, __LINE__, NULL})

    #define ERROR_CAUSE(code, msg, cause) \
        (&(Error){(code), (msg), __FILE__, __LINE__, (cause)})
