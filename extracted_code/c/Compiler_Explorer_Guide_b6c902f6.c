/*
 * Auto-generated from: 14_Video_Tutorials\02_Compiler_Explorer_Integration\Compiler_Explorer_Guide.md
 * Line: 389
 * Language: c
 * Block ID: b6c902f6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int global;

void by_value(int x) {
    global = x;
}

void by_pointer(int *x) {
    global = *x;
}

void by_reference(int &x) {
    global = x;
}
