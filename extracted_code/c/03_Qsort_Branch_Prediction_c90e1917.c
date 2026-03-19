/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\03_Qsort_Branch_Prediction.md
 * Line: 82
 * Language: c
 * Block ID: c90e1917
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 现代CPU的分支预测：
 *
 * 1. 静态预测：
 *    - 向后跳转（循环）预测为执行
 *    - 向前跳转预测为不执行
 *
 * 2. 动态预测：
 *    - 1-bit预测器：上次结果
 *    - 2-bit预测器：状态机（强/弱 执行/不执行）
 *    - 两级自适应：全局/局部分支历史
 *    - 锦标赛预测器：多预测器竞争
 *
 * 预测失败惩罚：15-20周期（深度流水线）
 */

// 分支预测友好的代码模式

// 模式1：可预测的分支
void predictable_branch(int *arr, int n) {
    // 查找第一个大于100的元素
    for (int i = 0; i < n; i++) {
        // 大多数元素小于100，分支预测"不执行"成功
        if (arr[i] > 100) {
            return i;
        }
    }
}

// 模式2：不可预测的分支（排序比较）
int unpredictable_compare(const void *a, const void *b) {
    int x = *(const int*)a;
    int y = *(const int*)b;

    // 随机数据导致50%概率，无法预测
    return (x > y) - (x < y);
}

// 分支预测统计
void profile_branch_prediction(void (*cmp)(void), int iterations) {
    // 使用性能计数器
    // Linux: perf_event_open

    struct perf_event_attr pe = {
        .type = PERF_TYPE_HARDWARE,
        .config = PERF_COUNT_HW_BRANCH_MISSES,
        .size = sizeof(struct perf_event_attr),
        .disabled = 1,
        .exclude_kernel = 1
    };

    int fd = perf_event_open(&pe, 0, -1, -1, 0);
    ioctl(fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

    // 运行代码...
    for (int i = 0; i < iterations; i++) {
        cmp();
    }

    ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);

    long long count;
    read(fd, &count, sizeof(count));

    printf("Branch misses: %lld\n", count);
    close(fd);
}
