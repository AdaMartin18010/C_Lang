/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\02_Searching_Algorithms.md
 * Line: 1363
 * Language: c
 * Block ID: 45315296
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 测试主函数
int main(void) {
    const char *text = "ABABDABACDABABCABAB";
    const char *pattern = "ABABCABAB";

    printf("=== 字符串搜索算法演示 ===\n\n");
    printf("文本: %s\n", text);
    printf("模式: %s\n\n", pattern);

    int pos;

    pos = kmp_search(text, pattern);
    printf("KMP: 模式出现在索引 %d\n", pos);

    pos = boyer_moore_simple(text, pattern);
    printf("Boyer-Moore: 模式出现在索引 %d\n", pos);

    pos = rabin_karp_search(text, pattern);
    printf("Rabin-Karp: 模式出现在索引 %d\n", pos);

    // 所有匹配位置
    int results[10];
    int count;
    kmp_search_all(text, "ABA", results, &count, 10);
    printf("\nKMP查找所有\"ABA\": 找到 %d 个，位置 [", count);
    for (int i = 0; i < count; i++) {
        printf("%d%s", results[i], i < count - 1 ? ", " : "");
    }
    printf("]\n");

    return 0;
}
