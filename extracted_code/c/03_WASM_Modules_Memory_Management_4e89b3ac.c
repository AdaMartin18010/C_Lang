/*
 * Auto-generated from: 10_WebAssembly_C\03_WASM_Modules_Memory_Management.md
 * Line: 1933
 * Language: c
 * Block ID: 4e89b3ac
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ========== 最佳实践准则 ==========

/*
1. 谁分配，谁释放
   - 在模块边界处明确内存所有权
   - 文档化内存管理责任

2. 使用 RAII 模式
   - 构造函数分配资源
   - 析构函数释放资源

3. 避免裸指针
   - 使用智能指针（在 C++ 中）
   - 或实现引用计数

4. 验证分配结果
   - 总是检查 malloc 返回值
   - 有优雅的错误处理

5. 限制分配频率
   - 批量分配而非多次小分配
   - 使用内存池重用内存

6. 避免内存碎片
   - 分配相似大小的块
   - 按相反顺序释放

7. 监控内存使用
   - 实现内存统计
   - 定期检查泄漏
*/

// 良好的内存管理示例

typedef struct {
    float* data;
    int size;
    int capacity;
    int ref_count;
} Buffer;

Buffer* buffer_create(int capacity) {
    Buffer* buf = (Buffer*)malloc(sizeof(Buffer));
    if (!buf) return NULL;

    buf->data = (float*)malloc(capacity * sizeof(float));
    if (!buf->data) {
        free(buf);
        return NULL;
    }

    buf->size = 0;
    buf->capacity = capacity;
    buf->ref_count = 1;
    return buf;
}

void buffer_retain(Buffer* buf) {
    if (buf) buf->ref_count++;
}

void buffer_release(Buffer* buf) {
    if (buf && --buf->ref_count == 0) {
        free(buf->data);
        free(buf);
    }
}
