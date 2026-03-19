/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\02_Dynamic_Linking_Category.md
 * Line: 154
 * Language: c
 * Block ID: 1ad035cf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// GOT访问的C代码示例
void got_access_example(void) {
    // 编译器生成：
    // call printf@plt

    // PLT:
    // jmp [printf@got]
    // push index
    // jmp plt0

    printf("Hello\n");
}
