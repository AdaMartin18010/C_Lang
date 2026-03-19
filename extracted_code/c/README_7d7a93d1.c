/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\README.md
 * Line: 123
 * Language: c
 * Block ID: 7d7a93d1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

   // Kconfig 驱动的条件编译
   #ifdef CONFIG_SMP
   // 多处理器支持代码
   static DEFINE_PER_CPU(struct task_struct *, curr_task);
   #else
   // 单处理器简化实现
   static struct task_struct *curr_task;
   #endif
   