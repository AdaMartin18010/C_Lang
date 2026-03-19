/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\02_Speculative_Execution.md
 * Line: 202
 * Language: c
 * Block ID: 85a8f03f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Spectre V2：污染分支目标缓冲区(BTB)

// 受害者代码
void (*indirect_call_target)(void);

void victim_function(void) {
    // 攻击者训练BTB使其预测到恶意目标
    indirect_call_target();  // 推测执行到错误地址
}

// 防护措施：Retpoline
/*
Retpoline汇编：
    call retpoline_call_target
    ...
retpoline_call_target:
    call set_up_target
capture_ret_spec:
    pause
    jmp capture_ret_spec
    ...
set_up_target:
    mov %rax, (%rsp)  ; 修改返回地址
    ret
*/
