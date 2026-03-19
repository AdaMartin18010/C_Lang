/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\01_EDAC_Memory.md
 * Line: 406
 * Language: c
 * Block ID: ddf1d35e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ Scrub间隔太长，多位错误无法纠正
#define SCRUB_INTERVAL  10000  // 10秒太长了!

// ✅ 根据SEU率计算间隔
// 如果SEU率 = 1e-6 errors/bit/day, 内存1MB
// 预期每天1个错误，Scrub间隔应<1小时
#define SCRUB_INTERVAL  300000  // 5分钟
