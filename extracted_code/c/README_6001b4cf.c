/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\README.md
 * Line: 243
 * Language: c
 * Block ID: 6001b4cf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Windows 风格 - 创建线程
#include <windows.h>

DWORD WINAPI thread_function(LPVOID arg) {
    int* value = (int*)arg;
    printf("Thread value: %d\n", *value);
    return 0;
}

int main() {
    HANDLE thread;
    DWORD threadId;
    int value = 42;
    thread = CreateThread(NULL, 0, thread_function, &value, 0, &threadId);
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
    return 0;
}
