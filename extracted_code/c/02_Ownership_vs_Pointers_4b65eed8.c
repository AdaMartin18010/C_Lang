/*
 * Auto-generated from: 15_Memory_Safe_Languages\01_Rust_vs_C\02_Ownership_vs_Pointers.md
 * Line: 173
 * Language: c
 * Block ID: 4b65eed8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C: UAF漏洞示例
char* get_message() {
    char* msg = malloc(100);
    strcpy(msg, "Hello, World!");
    return msg;  // 返回堆内存指针
}

void process() {
    char* message = get_message();
    printf("%s\n", message);
    free(message);

    // 危险：message仍然可访问，但已无效
    printf("After free: %s\n", message);  // UAF! 未定义行为

    // 更糟糕：可能被攻击者利用
    // free(message);  // 双重释放
}
