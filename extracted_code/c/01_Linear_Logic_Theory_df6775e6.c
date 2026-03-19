/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\01_Linear_Logic_Theory.md
 * Line: 167
 * Language: c
 * Block ID: df6775e6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 张量引入（右规则）
// 给定A的证明和B的证明，构造A⊗B的证明
Tensor *tensor_intro(Proof *proof_A, Proof *proof_B) {
    // 合并两个上下文
    return tensor_create(
        extract_resource(proof_A),
        extract_resource(proof_B)
    );
}

// 张量消去（左规则）
// 从A⊗B和(A,B ⊢ C)推导C
Resource *tensor_elim(Tensor *tensor,
                       Resource *(*cont)(Resource *, Resource *)) {
    // 解构张量
    Resource *a = tensor->first;
    Resource *b = tensor->second;
    // tensor现在无效
    free(tensor);
    // 继续计算
    return cont(a, b);
}
