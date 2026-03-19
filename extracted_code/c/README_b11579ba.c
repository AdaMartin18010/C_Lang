/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\README.md
 * Line: 300
 * Language: c
 * Block ID: b11579ba
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* core/game_loop.c */
#include "game_loop.h"
#include "time.h"
#include <stdio.h>

game_loop_t* game_loop_create(double target_fps, double fixed_hz) {
    game_loop_t* loop = malloc(sizeof(game_loop_t));
    loop->target_fps = target_fps;
    loop->fixed_delta_time = 1.0 / fixed_hz;
    loop->max_delta_time = 0.25;  /* 最大 250ms */
    loop->current_time = time_get_seconds();
    loop->accumulator = 0.0;
    loop->frame_count = 0;
    loop->fps_update_interval = 0.5;
    loop->last_fps_update = loop->current_time;
    loop->current_fps = 0;
    return loop;
}

void game_loop_run(game_loop_t* loop,
                    void (*update)(double),
                    void (*render)(double),
                    bool (*should_quit)(void)) {
    while (!should_quit()) {
        double new_time = time_get_seconds();
        double frame_time = new_time - loop->current_time;
        loop->current_time = new_time;

        /* 防止时间跳跃过大 */
        if (frame_time > loop->max_delta_time) {
            frame_time = loop->max_delta_time;
        }

        loop->accumulator += frame_time;

        /* 固定时间步长更新 - 保证物理确定性 */
        while (loop->accumulator >= loop->fixed_delta_time) {
            update(loop->fixed_delta_time);
            loop->accumulator -= loop->fixed_delta_time;
        }

        /* 插值系数 - 用于平滑渲染 */
        double alpha = loop->accumulator / loop->fixed_delta_time;
        render(alpha);

        /* FPS 计算 */
        loop->frame_count++;
        if (loop->current_time - loop->last_fps_update >= loop->fps_update_interval) {
            loop->current_fps = (int)(loop->frame_count /
                (loop->current_time - loop->last_fps_update));
            loop->frame_count = 0;
            loop->last_fps_update = loop->current_time;
            printf("FPS: %d\n", loop->current_fps);
        }

        /* 帧率限制 */
        if (loop->target_fps > 0) {
            double target_frame_time = 1.0 / loop->target_fps;
            double sleep_time = target_frame_time - (time_get_seconds() - new_time);
            if (sleep_time > 0) {
                time_sleep(sleep_time);
            }
        }
    }
}
