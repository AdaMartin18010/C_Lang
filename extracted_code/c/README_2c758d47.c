/*
 * Auto-generated from: 03_System_Technology_Domains\12_Persistent_Memory\README.md
 * Line: 486
 * Language: c
 * Block ID: 2c758d47
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* PMDK 类型系统 */
TOID(struct my_struct) obj;  /* 类型安全的对象 ID */

D_RO(obj)  /* 只读访问 */
D_RW(obj)  /* 读写访问 */
D_RO_DIRECT(obj)  /* 直接指针访问 */

/* 类型安全的事务操作 */
TX_ADD(obj)           /* 添加对象到事务 */
TX_ADD_DIRECT(ptr)    /* 添加直接指针 */
TX_SET(obj, field, value)  /* 安全设置字段 */
TX_NEW(type)          /* 事务分配 */
TX_ALLOC(type, size)  /* 事务分配指定大小 */
TX_FREE(obj)          /* 事务释放 */
