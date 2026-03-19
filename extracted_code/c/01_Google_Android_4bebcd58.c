/*
 * Auto-generated from: 15_Memory_Safe_Languages\03_Industry_Cases\01_Google_Android.md
 * Line: 595
 * Language: c
 * Block ID: 4bebcd58
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 问题: 某些C++特性难以在FFI中使用
// 解决方案: 封装层模式

// C++侧: 提供C兼容接口
extern "C" {
    struct CppObject;
    CppObject* cpp_object_create();
    void cpp_object_destroy(CppObject* obj);
    int cpp_object_do_work(CppObject* obj, const char* input);
}

// Rust侧: 安全包装
pub struct CppObjectWrapper {
    ptr: *mut c_void,
}

impl Drop for CppObjectWrapper {
    fn drop(&mut self) {
        unsafe { cpp_object_destroy(self.ptr); }
    }
}
