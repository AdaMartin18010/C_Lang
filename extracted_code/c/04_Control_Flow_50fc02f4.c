/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\04_Control_Flow.md
 * Line: 661
 * Language: c
 * Block ID: 50fc02f4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：使用goto进行循环
int sum = 0;
int i = 0;
loop:
    if (i >= 10) goto end;
    sum += i;
    i++;
    goto loop;
end:
    return sum;

// ✅ 正确：使用循环结构
int sum = 0;
for (int i = 0; i < 10; i++) {
    sum += i;
}
return sum;

// ✅ 可接受：goto用于错误处理清理
int process(void) {
    void *resource1 = alloc1();
    if (!resource1) return -1;

    void *resource2 = alloc2();
    if (!resource2) goto cleanup1;

    void *resource3 = alloc3();
    if (!resource3) goto cleanup2;

    // 处理逻辑

    free(resource3);
cleanup2:
    free(resource2);
cleanup1:
    free(resource1);
    return result;
}
