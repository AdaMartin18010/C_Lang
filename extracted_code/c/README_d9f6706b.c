/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\08_Debugging_Tools\README.md
 * Line: 684
 * Language: c
 * Block ID: d9f6706b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 症状：程序随机崩溃，崩溃位置不固定
// 原因：缓冲区溢出破坏了堆元数据

void buggy_function() {
    char *buffer = malloc(100);

    // 错误：复制了超过100字节
    strcpy(buffer, "This is a very long string that exceeds "
                   "the allocated buffer size of 100 bytes...");

    free(buffer);  // 崩溃可能在这里，因为元数据被破坏
}

// 调试方法：
// 1. Valgrind: valgrind --tool=memcheck ./program
//    输出：Invalid write of size 1 at ...
//
// 2. ASan: 编译时加 -fsanitize=address
//    输出：ERROR: AddressSanitizer: heap-buffer-overflow
//
// 3. GDB: 设置 watchpoint 监视 malloc 返回的地址+100
//    (gdb) watch *(char*)(buffer + 100)
