/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\README.md
 * Line: 680
 * Language: c
 * Block ID: fff074d3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 博弈语义中的精炼关系
 *
 * 策略 σ 精炼策略 τ 当：
 * - σ 的所有合法移动 τ 也能做
 * - σ 的响应比 τ 更具体或相等
 */

#include <stdbool.h>
#include <string.h>

// 抽象行为类型
typedef struct {
    int move_set[10];
    int num_moves;
} Behavior;

// 精炼检查
bool refines(const Behavior *spec, const Behavior *impl)
{
    // 实现的每个移动都必须在规范允许范围内
    for (int i = 0; i < impl->num_moves; i++) {
        bool found = false;
        for (int j = 0; j < spec->num_moves; j++) {
            if (impl->move_set[i] == spec->move_set[j]) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    return true;
}

// 前序关系：impl ≤ spec
bool implements(const Behavior *impl, const Behavior *spec)
{
    return refines(spec, impl);
}
