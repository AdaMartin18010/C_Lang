/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\01_Game_Semantics_Theory.md
 * Line: 404
 * Language: c
 * Block ID: a533f7d4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误：回答非最近的提问
Position bad_position = {
    .moves = {
        {O, Question, "q1"},  // O提问1
        {P, Question, "q2"},  // P反问
        {O, Answer, "a1"},    // O回答1 - 错误！应该回答q2
    }
};

// ✅ 正确：回答最近的未回答提问
Position good_position = {
    .moves = {
        {O, Question, "q1"},
        {P, Question, "q2"},
        {O, Answer, "a2"},    // 回答q2
        {P, Answer, "a1"},    // 回答q1
    }
};
