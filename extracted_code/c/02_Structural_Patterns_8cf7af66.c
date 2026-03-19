/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\02_Structural_Patterns.md
 * Line: 328
 * Language: c
 * Block ID: 8cf7af66
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 复杂子系统 */
typedef struct { /* compiler frontend */ } lexer_t;
typedef struct { /* parser */ } parser_t;
typedef struct { /* semantic analyzer */ } semantic_t;
typedef struct { /* code generator */ } codegen_t;

typedef struct {
    lexer_t* lexer;
    parser_t* parser;
    semantic_t* semantic;
    codegen_t* codegen;
} compiler_subsystem_t;

/* 外观: 简化接口 */
typedef struct {
    compiler_subsystem_t* subsys;
} compiler_facade_t;

compiler_facade_t* compiler_new(void) {
    compiler_facade_t* f = malloc(sizeof(compiler_facade_t));
    f->subsys = malloc(sizeof(compiler_subsystem_t));
    f->subsys->lexer = lexer_create();
    f->subsys->parser = parser_create();
    f->subsys->semantic = semantic_create();
    f->subsys->codegen = codegen_create();
    return f;
}

bool compile_file(compiler_facade_t* compiler, const char* source,
                  const char* output) {
    /* 隐藏复杂流程 */
    tokens_t* tokens = lexer_tokenize(compiler->subsys->lexer, source);
    ast_t* ast = parser_parse(compiler->subsys->parser, tokens);

    if (!semantic_check(compiler->subsys->semantic, ast)) {
        return false;
    }

    ir_t* ir = semantic_to_ir(compiler->subsys->semantic, ast);
    codegen_generate(compiler->subsys->codegen, ir, output);

    return true;
}

/* 使用 */
void example_facade(void) {
    compiler_facade_t* c = compiler_new();
    compile_file(c, "main.c", "main.o");
}
