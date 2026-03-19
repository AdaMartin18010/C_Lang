/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\02_Lock_Free_Programming.md
 * Line: 68
 * Language: c
 * Block ID: dd0293eb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 1. 阻塞 (Blocking)
 *    - 使用锁，一个线程可能永远阻塞其他线程
 *
 * 2. 无锁 (Lock-Free)
 *    - 至少一个线程在有限步骤内完成操作
 *    - 系统整体总有进度
 *
 * 3. 无等待 (Wait-Free)
 *    - 每个线程在有限步骤内完成操作
 *    - 最强的进度保证
 *
 * 4. 无干扰 (Obstruction-Free)
 *    - 线程单独执行时能在有限步骤完成
 *    - 最弱的无锁保证
 */
