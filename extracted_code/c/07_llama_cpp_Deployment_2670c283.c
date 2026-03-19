/*
 * Auto-generated from: 11_Machine_Learning_C\07_llama_cpp_Deployment.md
 * Line: 144
 * Language: c
 * Block ID: 2670c283
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 量化级别选择建议
const char* recommend_quantization(int available_ram_gb, bool requires_high_quality) {
    if (requires_high_quality) {
        if (available_ram_gb >= 16) return "Q8_0";
        if (available_ram_gb >= 8) return "Q5_K_M";
        return "Q4_K_M";
    } else {
        if (available_ram_gb >= 8) return "Q5_K_M";
        if (available_ram_gb >= 4) return "Q4_K_M";
        return "Q3_K_S";
    }
}
