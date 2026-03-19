/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_Feature_Previews.md
 * Line: 98
 * Language: c
 * Block ID: cd822559
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 示例1: 搜索算法中的提前退出
bool find_element(int target, int matrix[][100], int rows, int cols) {
    search: for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (matrix[i][j] == target) {
                printf("Found at [%d][%d]\n", i, j);
                return true;
                // 或使用: break search; 然后处理结果
            }
        }
    }
    return false;
}

// 示例2: 带标签的while循环
bool process_streams(Stream streams[], int n) {
    processing: while (has_work()) {
        for (int i = 0; i < n; i++) {
            if (streams[i].error) {
                log_error("Stream %d failed", i);
                break processing;  // 退出整个处理循环
            }
            process(&streams[i]);
        }
    }
    return !has_errors();
}

// 示例3: do-while循环命名
do_outer: do {
    // 一些初始化
    do_inner: do {
        if (should_abort()) {
            break do_outer;
        }
    } while (inner_condition());
} while (outer_condition());

// 示例4: switch内部的循环控制
void handle_events(EventQueue* q) {
    main_loop: while (queue_not_empty(q)) {
        Event e = dequeue(q);
        switch (e.type) {
            case EVENT_QUIT:
                break main_loop;  // 退出while，不是switch!
            case EVENT_SKIP:
                continue main_loop;
            // ...
        }
    }
}
