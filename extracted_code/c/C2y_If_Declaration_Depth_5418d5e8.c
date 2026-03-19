/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 360
 * Language: c
 * Block ID: 5418d5e8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 链式 else if 声明
void find_resource(int priority) {
    if (Resource* r = try_acquire(HIGH_PRIORITY); r != NULL) {
        printf("Acquired high priority resource\n");
        use_resource(r);
        release(r);
    } else if (Resource* r = try_acquire(MEDIUM_PRIORITY); r != NULL) {
        // 注意：这里声明了新的 r，遮蔽了外层的 r（值为 NULL）
        printf("Acquired medium priority resource\n");
        use_resource(r);
        release(r);
    } else if (Resource* r = try_acquire(LOW_PRIORITY); r != NULL) {
        printf("Acquired low priority resource\n");
        use_resource(r);
        release(r);
    } else {
        printf("Failed to acquire any resource\n");
    }
    // 所有 r 都不可见
}
