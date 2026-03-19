/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\02_SQLite_Engineering.md
 * Line: 1389
 * Language: c
 * Block ID: 51475187
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* SQLite API 设计 */

// a) 简洁的 C API
// 核心只有不到 100 个函数

// b) 资源管理约定
// _open/_close, _prepare/_finalize 配对
// 使用对象句柄模式

// c) 错误处理
// 返回码 + 错误消息
// 线程安全的错误信息

// d) 扩展性
// 虚拟表接口
// 用户自定义函数
// 自定义排序规则
