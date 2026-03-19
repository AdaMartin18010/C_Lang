/*
 * Auto-generated from: 01_Core_Knowledge_System\11_Internationalization\README.md
 * Line: 235
 * Language: c
 * Block ID: e084c182
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 复数规则定义 (.po 文件中):
 *
 * nplurals=2; plural=(n != 1);
 *   - 英语、德语等: 单数(n=1) vs 复数
 *
 * nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);
 *   - 俄语、波兰语等复杂复数规则
 *
 * nplurals=1; plural=0;
 *   - 中文、日语: 无复数变化
 */

// 代码中使用
void show_item_count(int n) {
    // gettext 会根据当前 locale 的复数规则选择正确形式
    printf(ngettext(
        "%d item selected",    // 单数形式
        "%d items selected",   // 复数形式
        n                      // 计数
    ), n);
}
