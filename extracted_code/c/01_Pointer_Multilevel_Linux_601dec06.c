/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Multilevel_Linux.md
 * Line: 840
 * Language: c
 * Block ID: 601dec06
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

定义: 指针类型系统

设类型集合 T = {int, char, float, ..., struct X, ...}

定义指针类型构造函数 *: T → T*
    若 t ∈ T, 则 t* ∈ T* (一级指针类型)

定义多级指针类型递归:
    T^0 = T                    (0级: 基本类型)
    T^1 = {t* | t ∈ T}         (1级: 一级指针)
    T^2 = {t** | t* ∈ T^1}     (2级: 二级指针)
    ...
    T^n = {t*...* | t ∈ T, n个*} (n级: n级指针)

类型关系:
    ∀t ∈ T, ∀n ≥ 0: sizeof(t^n) = sizeof(void*)  // 所有指针同宽
                    sizeof(t^0) = sizeof(t)      // 基本类型大小
