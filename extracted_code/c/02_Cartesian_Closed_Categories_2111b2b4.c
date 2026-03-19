/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\02_Algebraic_Topology\02_Cartesian_Closed_Categories.md
 * Line: 472
 * Language: c
 * Block ID: 2111b2b4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * β归约的实现
 * (λx.M) N →β M[x := N]
 */

// 替换: 在term中将var替换为replacement
term_t *term_substitute(term_t *term, const char *var, term_t *replacement)
{
    switch (term->type) {
        case TERM_VAR:
            if (strcmp(term->data.var_name, var) == 0) {
                return replacement;
            }
            return term;

        case TERM_ABS:
            if (strcmp(term->data.abs.var, var) == 0) {
                // 变量被绑定，不替换
                return term;
            }
            // 递归替换body
            term_t *new_body = term_substitute(term->data.abs.body, var, replacement);
            return term_abs(term->data.abs.var, term->data.abs.var_type, new_body);

        case TERM_APP:
            term_t *new_fn = term_substitute(term->data.app.fn, var, replacement);
            term_t *new_arg = term_substitute(term->data.app.arg, var, replacement);
            return term_app(new_fn, new_arg);
    }
    return term;
}

// β归约一步
term_t *beta_reduce_once(term_t *term)
{
    if (term->type == TERM_APP && term->data.app.fn->type == TERM_ABS) {
        // (λx.M) N → M[x := N]
        term_t *abs = term->data.app.fn;
        term_t *arg = term->data.app.arg;
        return term_substitute(abs->data.abs.body, abs->data.abs.var, arg);
    }
    return term;
}

// 归约到范式 (简化)
term_t *beta_reduce(term_t *term, int max_steps)
{
    for (int i = 0; i < max_steps; i++) {
        term_t *reduced = beta_reduce_once(term);
        if (reduced == term) {
            return term;  // 无法继续归约
        }
        term = reduced;
    }
    return term;
}
