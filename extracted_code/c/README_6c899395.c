/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\12_Self_Modifying_Code\README.md
 * Line: 226
 * Language: c
 * Block ID: 6c899395
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 多态调用优化
// obj->method(args);

// 单态内联缓存 (Monomorphic IC)
if (obj->vtable == expected_vtable) {
    // 快速路径：直接调用缓存的方法
    cached_method(obj, args);
} else {
    // 慢路径：查找方法，更新缓存
    method = lookup(obj, "method");
    update_cache(obj->vtable, method);
    method(obj, args);
}

// 多态内联缓存 (Megamorphic IC)
// 使用跳表处理多个常见类型
switch (obj->vtable->id) {
    case VTABLE_A: method_A(obj, args); break;
    case VTABLE_B: method_B(obj, args); break;
    default: lookup_and_call(obj, args);
}
