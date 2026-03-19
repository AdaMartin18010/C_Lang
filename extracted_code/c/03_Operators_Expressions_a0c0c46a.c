/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\03_Operators_Expressions.md
 * Line: 154
 * Language: c
 * Block ID: a0c0c46a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 优先级表（从高到低）

// 1. 后缀运算符（最高）
arr[i]      // 下标
func()      // 调用
. ->        // 成员访问
++ --       // 后缀自增自减
(type)      // 复合字面量(C99)

// 2. 前缀运算符
++ --       // 前缀自增自减
+ -         // 一元正负
! ~         // 逻辑非、位非
* &         // 解引用、取地址
sizeof      // 大小
_Alignof    // 对齐(C11)

// 3. 乘除模
* / %

// 4. 加减
+ -

// 5. 移位
<< >>

// 6. 关系
< <= > >=

// 7. 相等
== !=

// 8. 位与
&

// 9. 位异或
^

// 10. 位或
|

// 11. 逻辑与
&&

// 12. 逻辑或
||

// 13. 条件
?:

// 14. 赋值
= += -= *= /= %= <<= >>= &= ^=

// 15. 逗号（最低）
,
