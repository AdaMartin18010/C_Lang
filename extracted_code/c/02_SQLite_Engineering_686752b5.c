/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\02_SQLite_Engineering.md
 * Line: 1298
 * Language: c
 * Block ID: 686752b5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* SQLite 可靠性策略 */

// a) 保守的内存管理
// - 所有分配都有上限检查
// - 使用内存分配失败注入测试
// - 页缓存使用固定大小分配

// b) 事务安全保证
// - 写前日志确保原子性
// - 同步点策略可配置
// - 自动恢复机制

// c) 防御性编程
#define NEVER(X) ((X)?(assert(0),1):0)
#define ALWAYS(X) ((X)?1:(assert(0),0))

// 所有指针解引用前检查
if( NEVER(pPtr==0) ) return SQLITE_ERROR;

// d) 代码覆盖测试
// - 100% 分支覆盖
// - MC/DC 覆盖（修改条件/决策覆盖）
// - 故障注入测试
