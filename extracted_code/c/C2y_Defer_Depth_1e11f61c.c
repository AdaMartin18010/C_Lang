/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 170
 * Language: c
 * Block ID: 1e11f61c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C++ RAII
void process_file_cpp(const std::string& filename) {
    std::ifstream file(filename);  // 构造函数打开文件
    // 文件会在函数退出时自动关闭（析构函数）

    std::unique_ptr<char[]> buffer(new char[1024]);
    // buffer会自动释放

    // 业务逻辑...
}  // 所有资源自动释放
