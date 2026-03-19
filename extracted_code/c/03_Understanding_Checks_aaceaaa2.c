/*
 * Auto-generated from: 06_Thinking_Representation\09_Bloom_Taxonomy\03_Understanding_Checks.md
 * Line: 849
 * Language: c
 * Block ID: aaceaaa2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

S3: 以下代码是否有问题？
    char *p = malloc(10);
    free(p);
    if (p) { *p = 'a'; }

答案: 有。free后p成为悬空指针，尽管检查p非NULL，
     解引用仍是未定义行为。
