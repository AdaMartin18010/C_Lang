/*
 * Auto-generated from: 03_System_Technology_Domains\02_Regex_Engine\02_Pike_VM.md
 * Line: 119
 * Language: c
 * Block ID: ab3353ca
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 线程表示NFA的一个执行状态
typedef struct Thread {
    Inst *pc;         // 程序计数器
    const char *sp;   // 字符串指针
    Submatch *sub;    // 捕获组
} Thread;

// 线程列表（当前活跃的所有线程）
typedef struct ThreadList {
    Thread *threads;
    int n;
} ThreadList;

// Pike VM匹配引擎
bool pike_vm_match(Inst *prog, const char *input) {
    ThreadList clist = {malloc(100 * sizeof(Thread)), 0};
    ThreadList nlist = {malloc(100 * sizeof(Thread)), 0};

    // 初始线程
    clist.threads[0] = (Thread){prog, input, NULL};
    clist.n = 1;

    // 按字符迭代
    for (const char *sp = input; ; sp++) {
        // 处理当前列表中的所有线程
        for (int i = 0; i < clist.n; i++) {
            Thread t = clist.threads[i];

            switch (t.pc->type) {
                case CHAR:
                    if (*sp == t.pc->c) {
                        // 匹配成功，创建新线程到下轮
                        nlist.threads[nlist.n++] = (Thread){
                            t.pc + 1, sp + 1, t.sub
                        };
                    }
                    break;

                case SPLIT:
                    // 分裂：两个分支都加入当前列表
                    clist.threads[clist.n++] = (Thread){
                        t.pc->sub.x, sp, t.sub
                    };
                    clist.threads[clist.n++] = (Thread){
                        t.pc->sub.y, sp, t.sub
                    };
                    break;

                case JMP:
                    // 跳转：新PC加入当前列表
                    clist.threads[clist.n++] = (Thread){
                        t.pc->target, sp, t.sub
                    };
                    break;

                case MATCH:
                    // 匹配成功！
                    free(clist.threads);
                    free(nlist.threads);
                    return true;

                case ANY:
                    // 匹配任意字符
                    nlist.threads[nlist.n++] = (Thread){
                        t.pc + 1, sp + 1, t.sub
                    };
                    break;
            }
        }

        // 交换列表
        ThreadList tmp = clist;
        clist = nlist;
        nlist = tmp;
        nlist.n = 0;

        if (*sp == '\0') break;
    }

    free(clist.threads);
    free(nlist.threads);
    return false;
}
