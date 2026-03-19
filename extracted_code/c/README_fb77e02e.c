/*
 * Auto-generated from: 04_Industrial_Scenarios\10_DNA_Storage\README.md
 * Line: 314
 * Language: c
 * Block ID: fb77e02e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 处理 DNA 存储特有错误（插入、删除、替换）
typedef enum {
    ERROR_NONE,
    ERROR_SUBSTITUTION,
    ERROR_INSERTION,
    ERROR_DELETION
} DNAErrorType;

// Levenshtein 距离计算用于纠错
int levenshtein_distance(const char *s1, const char *s2,
                         int len1, int len2) {
    int *prev = malloc((len2 + 1) * sizeof(int));
    int *curr = malloc((len2 + 1) * sizeof(int));

    for (int j = 0; j <= len2; j++) prev[j] = j;

    for (int i = 1; i <= len1; i++) {
        curr[0] = i;
        for (int j = 1; j <= len2; j++) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            int deletion = prev[j] + 1;
            int insertion = curr[j-1] + 1;
            int substitution = prev[j-1] + cost;

            curr[j] = deletion;
            if (insertion < curr[j]) curr[j] = insertion;
            if (substitution < curr[j]) curr[j] = substitution;
        }
        int *temp = prev;
        prev = curr;
        curr = temp;
    }

    int result = prev[len2];
    free(prev);
    free(curr);
    return result;
}
