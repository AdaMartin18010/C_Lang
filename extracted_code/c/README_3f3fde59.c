/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\README.md
 * Line: 109
 * Language: c
 * Block ID: 3f3fde59
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C++: RAII (资源获取即初始化)
class Student {
    std::string name_;
    int age_;
    std::vector<float> scores_;

public:
    Student(const std::string& name, int age)
        : name_(name), age_(age) {}

    // 析构函数自动释放成员资源
    ~Student() = default;

    // 禁用拷贝，允许移动 (类似 Rust 的所有权)
    Student(const Student&) = delete;
    Student& operator=(const Student&) = delete;
    Student(Student&&) = default;
    Student& operator=(Student&&) = default;
};
