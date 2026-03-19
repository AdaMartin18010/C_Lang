/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 385
 * Language: c
 * Block ID: d336e49e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 更好的做法：使用不同的变量名
void find_resource_clean(int priority) {
    if (Resource* high = try_acquire(HIGH_PRIORITY); high != NULL) {
        use_resource(high);
        release(high);
    } else if (Resource* medium = try_acquire(MEDIUM_PRIORITY); medium != NULL) {
        use_resource(medium);
        release(medium);
    } else if (Resource* low = try_acquire(LOW_PRIORITY); low != NULL) {
        use_resource(low);
        release(low);
    } else {
        printf("Failed to acquire any resource\n");
    }
}
