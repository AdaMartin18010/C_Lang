/*
 * Auto-generated from: 17_Ada_SPARK\03_Ada_C_Interoperability\01_Calling_C_from_Ada.md
 * Line: 87
 * Language: c
 * Block ID: 512c5571
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int string_operation(const char* input, char* output, size_t out_size) {
    if (strlen(input) >= out_size) return -1;
    strcpy(output, "Result: ");
    strcat(output, input);
    return 0;
}
