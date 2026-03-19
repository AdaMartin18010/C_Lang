/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\02_Autosar_Architecture.md
 * Line: 107
 * Language: c
 * Block ID: b85d0631
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Sender-Receiver接口（数据流） */
/* 发送端 */
Std_ReturnType Rte_Write_<p>_<o>(<data_type> data);
/* 接收端 */
Std_ReturnType Rte_Read_<p>_<o>(<data_type>* data);

/* Client-Server接口（函数调用） */
/* 客户端 */
Std_ReturnType Rte_Call_<p>_<o>(<args>);
/* 服务器 */
void Rte_<runnable_name>(void);
