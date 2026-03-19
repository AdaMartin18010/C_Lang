/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\04_CSP_Model_C.md
 * Line: 706
 * Language: c
 * Block ID: a7fe8537
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Fan-out: 一个输入，多个处理 */
void fan_out(chan_t* input, chan_t** outputs, int n) {
    for (int i = 0; i < n; i++) {
        go((void(*)(void*))fan_out_worker,
           &(fan_out_args){input, outputs[i]});
    }
}

void fan_out_worker(fan_out_args* args) {
    int value;
    while (chan_recv(args->input, &value)) {
        chan_send(args->output, &value);
    }
    chan_close(args->output);
}

/* Fan-in: 多个输入，一个输出 */
chan_t* fan_in(chan_t** inputs, int n) {
    chan_t* output = make_chan(sizeof(int), 0);

    for (int i = 0; i < n; i++) {
        go((void(*)(void*))fan_in_worker,
           &(fan_in_args){inputs[i], output});
    }

    /* 等待所有输入关闭后关闭输出 */
    go((void(*)(void*))fan_in_closer,
       &(fan_in_close_args){inputs, n, output});

    return output;
}

void fan_in_worker(fan_in_args* args) {
    int value;
    while (chan_recv(args->input, &value)) {
        chan_send(args->output, &value);
    }
}
