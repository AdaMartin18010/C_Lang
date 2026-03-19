/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\10_Debug_Info_Encoding\README.md
 * Line: 185
 * Language: c
 * Block ID: 1c3e117b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ELF 符号表条目（.symtab 和 .dynsym）
struct Elf64_Sym {
    uint32_t st_name;   // 字符串表索引
    uint8_t  st_info;   // 类型和绑定属性
    uint8_t  st_other;  // 可见性
    uint16_t st_shndx;  // 相关节的索引
    uint64_t st_value;  // 符号值（地址）
    uint64_t st_size;   // 符号大小
};

// 符号类型 (ELF32_ST_TYPE)
#define STT_NOTYPE  0   // 未指定类型
#define STT_OBJECT  1   // 数据对象（变量）
#define STT_FUNC    2   // 函数
#define STT_SECTION 3   // 节
#define STT_FILE    4   // 文件名
#define STT_COMMON  5   // 公共数据
#define STT_TLS     6   // 线程本地存储

// 符号绑定 (ELF32_ST_BIND)
#define STB_LOCAL   0   // 局部符号
#define STB_GLOBAL  1   // 全局符号
#define STB_WEAK    2   // 弱符号
