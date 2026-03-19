/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 415
 * Language: c
 * Block ID: 96d7321d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// if 声明适合的场景
void classify_value(int x) {
    // 复杂条件判断
    if (int abs_x = x >= 0 ? x : -x; abs_x < 10) {
        printf("Small value\n");
    } else if (abs_x < 100) {
        printf("Medium value\n");
    } else {
        printf("Large value\n");
    }
}

// switch 适合的场景
void handle_command(enum Cmd cmd) {
    switch (cmd) {
        case CMD_START:  start();  break;
        case CMD_STOP:   stop();   break;
        case CMD_PAUSE:  pause();  break;
        default:         error();  break;
    }
}
