/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\03_Hash_Table_Implementation.md
 * Line: 227
 * Language: c
 * Block ID: b9f3d7cb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * DJB2 哈希算法
 * Dan Bernstein 设计的简单高效字符串哈希
 */
unsigned int djb2_hash(const char* str) {
    unsigned int hash = 5381;
    int c;
    while ((c = (unsigned char)(*str++))) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }
    return hash;
}
