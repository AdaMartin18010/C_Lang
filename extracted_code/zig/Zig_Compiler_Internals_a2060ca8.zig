//
// Auto-generated from: Zig\Zig_Compiler_Internals.md
// Line: 31
// Language: zig
// Block ID: a2060ca8
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const TokenTag = enum {
    // 关键字
    keyword_const, keyword_var, keyword_fn,
    keyword_return, keyword_if, keyword_else,
    keyword_while, keyword_for, keyword_switch,
    keyword_try, keyword_catch, keyword_defer,
    keyword_comptime, keyword_pub,

    // 标识符和字面量
    identifier,
    string_literal,
    integer_literal,
    float_literal,

    // 运算符
    equal, equal_equal, bang_equal,
    less_than, greater_than,
    plus, minus, asterisk, slash,

    // 分隔符
    l_paren, r_paren,
    l_brace, r_brace,
    l_bracket, r_bracket,
    semicolon, comma, colon,

    eof, invalid,
};
