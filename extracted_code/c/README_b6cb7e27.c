/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\README.md
 * Line: 404
 * Language: c
 * Block ID: b6cb7e27
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C++ RAII 的线性逻辑解释

class LinearResource {
public:
    // 构造函数：创建资源
    LinearResource() : data(new int[1000]) {
        std::cout << "资源创建\n";
    }

    // 禁用拷贝（线性性质）
    LinearResource(const LinearResource&) = delete;
    LinearResource& operator=(const LinearResource&) = delete;

    // 允许移动（所有权转移）
    LinearResource(LinearResource&& other) noexcept
        : data(other.data) {
        other.data = nullptr;
        std::cout << "资源移动\n";
    }

    // 析构函数：资源消耗
    ~LinearResource() {
        delete[] data;
        std::cout << "资源释放\n";
    }

private:
    int* data;
};

// 使用模式
void use_linear() {
    LinearResource r1;           // 创建
    LinearResource r2 = std::move(r1);  // 所有权转移
}  // r2 被释放，r1 已空
