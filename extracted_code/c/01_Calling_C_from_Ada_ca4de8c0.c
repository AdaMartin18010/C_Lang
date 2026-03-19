/*
 * Auto-generated from: 17_Ada_SPARK\03_Ada_C_Interoperability\01_Calling_C_from_Ada.md
 * Line: 32
 * Language: c
 * Block ID: ca4de8c0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C函数定义
extern "C" {
    double calculate_area(double radius);
    int process_data(const char* input, char* output, size_t out_size);
    void* allocate_buffer(size_t size);
    void free_buffer(void* ptr);
}

double calculate_area(double radius) {
    return 3.14159 * radius * radius;
}
