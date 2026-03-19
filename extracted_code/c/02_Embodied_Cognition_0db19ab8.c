/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\02_Embodied_Cognition.md
 * Line: 382
 * Language: c
 * Block ID: 0db19ab8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 调试输出的感官增强

// 视觉：颜色编码
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_RESET   "\x1b[0m"

void log_with_color(LogLevel level, const char *msg) {
    const char *color = COLOR_RESET;
    switch (level) {
        case LOG_ERROR:   color = COLOR_RED;    break;
        case LOG_WARNING: color = COLOR_YELLOW; break;
        case LOG_SUCCESS: color = COLOR_GREEN;  break;
        default: break;
    }
    printf("%s[%s]%s %s\n", color, level_name(level), COLOR_RESET, msg);
}

// 听觉：重要事件的声音提示（伪代码）
void play_sound_alert(const char *event) {
    #ifdef AUDIO_ENABLED
    if (strcmp(event, "breakpoint_hit") == 0) {
        system("play bell.wav");
    } else if (strcmp(event, "error") == 0) {
        system("play error.wav");
    }
    #endif
}
