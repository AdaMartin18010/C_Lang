/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\12_Compiler_Optimization\README.md
 * Line: 250
 * Language: c
 * Block ID: 46169ddf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// LLVM 常量传播 Pass 简化实现
class ConstantPropagationPass : public FunctionPass {
  bool runOnFunction(Function &F) override {
    bool Changed = false;
    for (BasicBlock &BB : F) {
      for (Instruction &I : BB) {
        // 尝试常量折叠
        if (Constant *C = ConstantFoldInstruction(&I)) {
          I.replaceAllUsesWith(C);  // 替换所有使用
          I.eraseFromParent();       // 删除原指令
          Changed = true;
        }
      }
    }
    return Changed;
  }
};
