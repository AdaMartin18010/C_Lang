/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\04_Secure_Coding_Guide.md
 * Line: 2675
 * Language: c
 * Block ID: 02edaff7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * C代码安全审计检查清单
 * 
 * 复制此清单到每个源文件进行自查
 */

/* 
 * □ 内存管理
 *   - 所有malloc都有对应的free
 *   - 已检查分配失败
 *   - 无双重释放
 *   - 无使用-after-free
 *   - 敏感数据已用memset_s清零
 * 
 * □ 缓冲区
 *   - 所有字符串操作使用strlcpy/strlcat
 *   - 数组访问有边界检查
 *   - 整数运算检查溢出
 * 
 * □ 输入处理
 *   - 外部输入经过验证
 *   - 格式化字符串使用"%s", 而非直接传入
 *   - 无命令注入风险
 * 
 * □ 并发
 *   - 共享数据有适当同步
 *   - 无死锁风险
 *   - 原子操作使用正确
 * 
 * □ 错误处理
 *   - 所有错误路径正确处理
 *   - 资源在错误时释放
 *   - 不泄露系统信息
 * 
 * □ 密码学
 *   - 使用加密安全随机数
 *   - 密钥派生使用PBKDF2/Argon2
 *   - 敏感内存已锁定
 */
