/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Frama_C_LLM_Integration.md
 * Line: 2238
 * Language: c
 * Block ID: 697c9a27
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int count(int *arr, int n, int x) {
    int c = 0;
    // 需要不变量
    for (int i = 0; i < n; i++) {
        if (arr[i] == x) c++;
    }
    return c;
}
