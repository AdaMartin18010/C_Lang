/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\README.md
 * Line: 232
 * Language: c
 * Block ID: b9cae791
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 虚拟地址 → 物理地址转换
// 虚拟地址: [VPN (虚拟页号) | 页内偏移]
//               ↓
//           TLB 查找 (快)
//               ↓
// 物理地址: [PFN (物理页号) | 页内偏移]

// TLB 未命中示例
typedef struct {
    uint64_t tag;      // 虚拟页号部分
    uint64_t pfn;      // 物理页号
    uint8_t  flags;    // 权限位
    uint8_t  asid;     // 地址空间ID
} TLB_Entry;

// 大页减少 TLB 压力
void huge_pages_benefit() {
    // 4KB 页: 访问 1GB 需要 262144 个 TLB 项
    // 2MB 大页: 访问 1GB 只需要 512 个 TLB 项
    // 1GB 巨页: 访问 1GB 只需要 1 个 TLB 项
}
