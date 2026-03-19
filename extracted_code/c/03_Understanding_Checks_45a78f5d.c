/*
 * Auto-generated from: 06_Thinking_Representation\09_Bloom_Taxonomy\03_Understanding_Checks.md
 * Line: 942
 * Language: c
 * Block ID: 45a78f5d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

S14: 解释为什么malloc(0)的行为是实现定义的。

答案: C标准允许返回NULL或唯一可释放指针。
     不同实现行为不同，应始终检查返回值。
