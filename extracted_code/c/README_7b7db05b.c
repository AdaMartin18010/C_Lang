/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\README.md
 * Line: 267
 * Language: c
 * Block ID: 7b7db05b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* core/game_loop.h */
#ifndef GAME_LOOP_H
#define GAME_LOOP_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    double target_fps;
    double fixed_delta_time;    /* 固定物理更新间隔 */
    double max_delta_time;      /* 防止卡顿导致的时间跳跃 */

    /* 运行时数据 */
    double current_time;
    double accumulator;         /* 物理更新累积器 */
    uint64_t frame_count;
    double fps_update_interval;
    double last_fps_update;
    int current_fps;
} game_loop_t;

game_loop_t* game_loop_create(double target_fps, double fixed_hz);
void game_loop_run(game_loop_t* loop,
                    void (*update)(double),      /* 固定频率更新 */
                    void (*render)(double),      /* 插值渲染 */
                    bool (*should_quit)(void));

#endif
