/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\03_Denotational_Semantics.md
 * Line: 618
 * Language: c
 * Block ID: c3ca0bc7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 简单常量传播分析
 * 基于指称语义的抽象解释
 */

typedef enum { UNKNOWN, KNOWN } ConstStatus;

typedef struct {
    ConstStatus status;
    int value;
} ConstProp;

ConstProp eval_const(ConstProp a, ConstProp b, char op) {
    if (a.status == KNOWN && b.status == KNOWN) {
        int result;
        switch (op) {
            case '+': result = a.value + b.value; break;
            case '-': result = a.value - b.value; break;
            case '*': result = a.value * b.value; break;
            default: return (ConstProp){UNKNOWN, 0};
        }
        return (ConstProp){KNOWN, result};
    }
    return (ConstProp){UNKNOWN, 0};
}
