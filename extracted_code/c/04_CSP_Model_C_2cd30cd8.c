/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\04_CSP_Model_C.md
 * Line: 813
 * Language: c
 * Block ID: 2cd30cd8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 埃拉托斯特尼筛法的并发实现 */

void filter(chan_t* in, chan_t* out, int prime) {
    int n;
    while (chan_recv(in, &n)) {
        if (n % prime != 0) {
            chan_send(out, &n);
        }
    }
    chan_close(out);
}

void sieve(void) {
    chan_t* ch = make_chan(sizeof(int), 100);

    /* 生成数字 */
    go((void(*)(void*))generate, ch);

    /* 筛法管道 */
    for (int i = 0; i < 10; i++) {
        int prime;
        if (!chan_recv(ch, &prime)) {
            break;
        }
        printf("%d ", prime);

        chan_t* next = make_chan(sizeof(int), 100);
        go((void(*)(void*))filter, &(filter_args){ch, next, prime});
        ch = next;
    }
    printf("\n");
}

void generate(chan_t* ch) {
    for (int i = 2; ; i++) {
        chan_send(ch, &i);
    }
}
