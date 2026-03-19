/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\04_C23_Memory_Security.md
 * Line: 182
 * Language: c
 * Block ID: 5e6e1762
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Checked C头文件
#include <stdchecked.h>

// ptr<T>: 单个对象的指针
ptr<int> p = &x;
*p = 42;  // 自动边界检查

// array_ptr<T>: 指向数组的指针
array_ptr<int> arr : count(n);
// arr[0]到arr[n-1]是有效的

// nt_array_ptr<T>: 以null结尾的数组
nt_array_ptr<char> str : count(len);

// 边界表达式
array_ptr<int> buffer : byte_count(size * sizeof(int));
array_ptr<int> slice : bounds(low, high);
