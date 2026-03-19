/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\README.md
 * Line: 294
 * Language: c
 * Block ID: 26736bb8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 明确所有权语义

// 转移所有权 (sink)
void container_take_ownership(container_t* c, item_t* item);

// 借用 (不转移所有权)
const item_t* container_borrow(const container_t* c, size_t index);

// 复制 (创建新对象)
item_t* item_clone(const item_t* src);

// 共享所有权 (引用计数)
void shared_ptr_acquire(shared_ptr_t* ptr);
void shared_ptr_release(shared_ptr_t* ptr);
