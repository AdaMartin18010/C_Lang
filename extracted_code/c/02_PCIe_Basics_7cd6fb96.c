/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\10_IO_System\02_PCIe_Basics.md
 * Line: 534
 * Language: c
 * Block ID: 7cd6fb96
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * PCIe事务排序规则 (Ordering Rules)
 * 确保正确性和性能
 */

// 事务类别:
// 1. Posted (P): MWr, Msg - 无需响应
// 2. Non-Posted (NP): MRd, IORd, CfgRd - 需要完成包
// 3. Completion (Cpl): Cpl, CplD - 响应包

// 排序规则表 (简化):
//           | 后续 P  | 后续 NP | 后续 Cpl
// 前序 P    |  Y(1)   |   Y     |   Y
// 前序 NP   |  N(2)   |   Y     |   Y
// 前序 Cpl  |  Y      |   Y     |   Y

// (1) 允许，但可能乱序 ( relaxed ordering )
// (2) 不允许，避免死锁

// Linux PCIe驱动中的排序考虑
static inline void pcie_set_relaxed_ordering(struct pci_dev *pdev, bool enable)
{
    u16 old, new;

    // 修改设备控制寄存器的 relaxed ordering 位
    pcie_capability_read_word(pdev, PCI_EXP_DEVCTL, &old);

    new = old;
    if (enable)
        new |= PCI_EXP_DEVCTL_RELAX_EN;
    else
        new &= ~PCI_EXP_DEVCTL_RELAX_EN;

    if (new != old)
        pcie_capability_write_word(pdev, PCI_EXP_DEVCTL, new);
}

// 流量控制 (Flow Control)
// 每个VC有6个独立的FC信用:
// - Posted Header (PH)
// - Posted Data (PD)
// - Non-Posted Header (NPH)
// - Non-Posted Data (NPD)
// - Completion Header (CPLH)
// - Completion Data (CPLD)

// 发送前必须检查信用可用性
struct pcie_fc_credits {
    u16 ph;     // Posted Header
    u16 pd;     // Posted Data
    u16 nph;    // Non-Posted Header
    u16 npd;    // Non-Posted Data
    u16 cplh;   // Completion Header
    u16 cpld;   // Completion Data
};
