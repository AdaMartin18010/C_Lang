/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\01_Game_Semantics_Theory.md
 * Line: 427
 * Language: c
 * Block ID: 1fd4c231
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 非确定性策略：同一位置两个不同响应
Strategy bad_strategy = {
    .positions = {
        { /* 空位置 */ },
        { {O, Question, "q"} },
    },
    .responses = {
        NULL,  // 空位置
        (Move[]){ {P, Answer, "0"}, {P, Answer, "1"} }  // 两个答案！
    }
};

// ✅ 确定性策略：每个位置唯一响应
Strategy good_strategy = {
    .responses = {
        NULL,
        (Move[]){ {P, Answer, "0"} }  // 唯一答案
    }
};
