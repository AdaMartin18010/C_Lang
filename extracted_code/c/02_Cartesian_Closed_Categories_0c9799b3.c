/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\02_Algebraic_Topology\02_Cartesian_Closed_Categories.md
 * Line: 597
 * Language: c
 * Block ID: 0c9799b3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 函子(Functor)和单子(Monad)的C表达
 */

// Maybe函子: 部分函数的计算
typedef struct maybe {
    bool is_just;
    void *value;
} maybe_t;

#define NOTHING ((maybe_t){.is_just = false, .value = NULL})
#define JUST(x) ((maybe_t){.is_just = true, .value = (x)})

// fmap : (a → b) → Maybe a → Maybe b
maybe_t maybe_fmap(void *(*f)(void *), maybe_t m)
{
    if (!m.is_just) return NOTHING;
    return JUST(f(m.value));
}

// bind (>>=) : Maybe a → (a → Maybe b) → Maybe b
maybe_t maybe_bind(maybe_t m, maybe_t (*f)(void *))
{
    if (!m.is_just) return NOTHING;
    return f(m.value);
}

// return/pure : a → Maybe a
maybe_t maybe_return(void *x)
{
    return JUST(x);
}

// 状态单子模拟
typedef struct {
    int value;
} state_t;

typedef struct {
    state_t (*run)(void *value, state_t state);
    void *value;
} state_monad_t;

// 纯函数式状态操作
state_monad_t state_return(void *value)
{
    state_monad_t m = {
        .run = lambda(void *, (void *v, state_t s) { return s; }),
        .value = value
    };
    return m;
}

// 简化宏定义辅助
#define lambda(ret, body) ((ret (*)(void *, state_t))body)
