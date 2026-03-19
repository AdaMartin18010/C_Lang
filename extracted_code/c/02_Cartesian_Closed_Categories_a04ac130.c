/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\02_Algebraic_Topology\02_Cartesian_Closed_Categories.md
 * Line: 283
 * Language: c
 * Block ID: a04ac130
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 类型构造器的范畴论解释
 */

// 单位类型 (终对象)
typedef struct {} unit_t;

// Void类型 (始对象) - 无值
typedef void void_t;

// 积类型: A ∧ B ≅ A × B
typedef struct {
    object_t *fst;
    object_t *snd;
} pair_t;

pair_t pair_make(object_t *a, object_t *b)
{
    return (pair_t){.fst = a, .snd = b};
}

// 函数类型: A → B ≅ B^A
// 在C中就是函数指针

// 和类型 (余积): A ∨ B ≅ A + B
typedef struct {
    enum { LEFT_TAG, RIGHT_TAG } tag;
    union {
        object_t left;
        object_t right;
    } data;
} either_t;

either_t either_left(object_t *a)
{
    return (either_t){.tag = LEFT_TAG, .data.left = *a};
}

either_t either_right(object_t *b)
{
    return (either_t){.tag = RIGHT_TAG, .data.right = *b};
}

// 和类型的fold (catamorphism)
object_t either_fold(either_t e,
                      morphism_t left_fn,
                      morphism_t right_fn)
{
    if (e.tag == LEFT_TAG) {
        return left_fn(&e.data.left);
    } else {
        return right_fn(&e.data.right);
    }
}
