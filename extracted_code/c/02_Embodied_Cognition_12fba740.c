/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\02_Embodied_Cognition.md
 * Line: 121
 * Language: c
 * Block ID: 12fba740
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 调试的心智过程模型

/*
调试认知循环：

1. 观察 (Observation)
   - 看到异常输出
   - 注意到未预期的行为

2. 假设 (Hypothesis)
   - 猜测可能的原因
   - 形成解释模型

3. 实验 (Experiment)
   - 设置断点
   - 添加日志
   - 修改输入

4. 验证 (Verification)
   - 检查假设是否成立
   - 确认或否定

5. 修复 (Fix)
   - 实施修正
   - 验证修复
*/

// 代码示例：使用断言支持假设验证
void process_data(Data *d) {
    // 观察点：记录输入状态
    log_debug("Processing data: id=%d, size=%zu", d->id, d->size);

    // 假设：数据应该已经验证
    assert(d->magic == DATA_MAGIC && "Data should be validated");

    // 实验：检查边界条件
    if (d->size > MAX_SIZE) {
        // 假设验证失败，需要调查
        log_error("Size %zu exceeds maximum %zu", d->size, MAX_SIZE);
        return;
    }

    // 继续处理...
}
