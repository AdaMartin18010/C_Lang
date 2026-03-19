/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\04_C23_Memory_Security.md
 * Line: 109
 * Language: c
 * Block ID: 3ebb42e7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23: nullptr不能转整数，防止意外调用错误函数
void func_int(int x);
void func_ptr(void *p);

func_int(NULL);      // ⚠️ 可能通过 (依赖实现)
// func_int(nullptr); // ✅ 编译错误! nullptr不能转int
func_ptr(nullptr);   // ✅ 明确调用指针版本
