/*
 * Auto-generated from: 01_Core_Knowledge_System\03_Construction_Layer\01_Structures_Unions.md
 * Line: 237
 * Language: c
 * Block ID: 2de7d164
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 危险：假设无填充
struct Header {
    uint8_t type;
    uint32_t length;
};

void write_header(FILE *fp, struct Header *h) {
    fwrite(h, sizeof(*h), 1, fp);  // 包含填充字节！
}

// ✅ 正确：序列化时手动打包
void write_header_safe(FILE *fp, struct Header *h) {
    fwrite(&h->type, 1, 1, fp);
    fwrite(&h->length, 4, 1, fp);
}
