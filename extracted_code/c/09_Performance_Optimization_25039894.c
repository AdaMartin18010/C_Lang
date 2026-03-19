/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\09_Performance_Optimization.md
 * Line: 313
 * Language: c
 * Block ID: 25039894
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// KMP算法 (线性时间字符串匹配)
void compute_lps(const char *pat, int m, int *lps);

void kmp_search(const char *txt, const char *pat) {
    int n = strlen(txt);
    int m = strlen(pat);
    int lps[m];
    compute_lps(pat, m, lps);

    int i = 0, j = 0;
    while (i < n) {
        if (pat[j] == txt[i]) {
            i++; j++;
        }
        if (j == m) {
            printf("Found at %d\n", i - j);
            j = lps[j - 1];
        } else if (i < n && pat[j] != txt[i]) {
            if (j != 0) j = lps[j - 1];
            else i++;
        }
    }
}

// Boyer-Moore (更快的平均情况)
void boyer_moore_search(const char *txt, const char *pat);
