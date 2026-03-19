/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\04_CSP_Model_C.md
 * Line: 749
 * Language: c
 * Block ID: 621f0e80
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 流水线阶段 */
typedef chan_t* (*stage_fn)(chan_t*);

chan_t* pipeline(chan_t* input, stage_fn* stages, int n) {
    chan_t* current = input;

    for (int i = 0; i < n; i++) {
        current = stages[i](current);
    }

    return current;
}

/* 示例阶段 */
chan_t* stage_filter(chan_t* input) {
    chan_t* output = make_chan(sizeof(int), 10);

    go((void(*)(void*))filter_worker, &(filter_args){input, output});

    return output;
}

void filter_worker(filter_args* args) {
    int value;
    while (chan_recv(args->input, &value)) {
        if (value % 2 == 0) {  /* 过滤偶数 */
            chan_send(args->output, &value);
        }
    }
    chan_close(args->output);
}

/* 使用 */
void example_pipeline(void) {
    chan_t* input = make_chan(sizeof(int), 10);

    stage_fn stages[] = {
        stage_filter,
        stage_square,
        stage_sum
    };

    chan_t* output = pipeline(input, stages, 3);

    /* 发送数据 */
    for (int i = 1; i <= 100; i++) {
        chan_send(input, &i);
    }
    chan_close(input);

    /* 接收结果 */
    int result;
    chan_recv(output, &result);
    printf("Sum: %d\n", result);
}
