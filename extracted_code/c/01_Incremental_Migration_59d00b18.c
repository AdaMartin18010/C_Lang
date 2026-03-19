/*
 * Auto-generated from: 15_Memory_Safe_Languages\02_Migration_Strategies\01_Incremental_Migration.md
 * Line: 168
 * Language: c
 * Block ID: 59d00b18
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 类型A: 优先迁移 - 纯计算逻辑，无复杂系统依赖
// 安全关键，易于验证
int parse_config(const char* input, Config* output) {
    // 解析逻辑，边界检查密集
    // 适合完全迁移到Rust
}

// 类型B: 条件迁移 - 依赖系统API，需要FFI包装
// 需要仔细的边界设计
ssize_t read_data(int fd, void* buf, size_t len) {
    // IO操作，需要保持C接口
    // Rust实现，C兼容API
}

// 类型C: 延后迁移 - 硬件相关，性能极度敏感
// 除非有充分测试，否则保持C
void* dma_alloc(size_t size, uint32_t flags) {
    // 直接硬件操作
    // 需要专业硬件验证
}
