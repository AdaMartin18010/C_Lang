/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\01_Linear_Logic_Theory.md
 * Line: 266
 * Language: c
 * Block ID: 18a44878
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 非Curried: (A ⊗ B) ⊸ C
Resource *uncurried(Tensor *ab) {
    Resource *a = ab->first;
    Resource *b = ab->second;
    free(ab);
    return operation(a, b);
}

// Curried: A ⊸ (B ⊸ C)
LinearFunction *curried(Resource *a) {
    // 返回B ⊸ C的闭包
    LinearFunction *f = malloc(sizeof(LinearFunction));
    f->closure = a;  // 捕获a
    f->apply = lambda(Resource *, (void *c, Resource *b) {
        Resource *a = (Resource *)c;
        return operation(a, b);
    });
    return f;
}
