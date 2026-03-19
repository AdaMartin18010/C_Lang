/*
 * Auto-generated from: 04_Industrial_Scenarios\10_DNA_Storage\README.md
 * Line: 190
 * Language: c
 * Block ID: facdb768
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 旋转码确保 GC 含量在 50% 左右
void rotate_encode(const uint8_t *data, size_t len,
                   char *output, size_t out_cap) {
    const char *rotations[] = {
        "ACGT",  // 00 -> ACGT
        "CATG",  // 01 -> CATG
        "GTAC",  // 10 -> GTAC
        "TGCA"   // 11 -> TGCA
    };

    size_t out_idx = 0;
    int rotation_idx = 0;

    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];
        for (int j = 3; j >= 0; j--) {
            uint8_t bits = (byte >> (j * 2)) & 0x03;
            if (out_idx >= out_cap) return;
            output[out_idx++] = rotations[rotation_idx][bits];
            rotation_idx = (rotation_idx + 1) % 4;
        }
    }
    output[out_idx] = '\0';
}
