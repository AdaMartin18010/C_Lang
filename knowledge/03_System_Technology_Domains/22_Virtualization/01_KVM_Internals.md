# KVM 虚拟化内幕

> **层级**: L5 (原理层)
> **目标**: 深入理解 Linux KVM 虚拟化架构、硬件辅助机制与性能优化
> **更新日期**: 2026-03-28
> **适用版本**: Linux 6.2+ / QEMU 8.0+

---

## 目录

- [KVM 虚拟化内幕](#kvm-虚拟化内幕)
  - [目录](#目录)
  - [本节概要](#本节概要)
  - [虚拟化技术概述](#虚拟化技术概述)
    - [虚拟化技术演进](#虚拟化技术演进)
    - [虚拟化技术对比](#虚拟化技术对比)
  - [KVM架构与核心组件](#kvm架构与核心组件)
    - [KVM整体架构](#kvm整体架构)
    - [核心组件职责](#核心组件职责)
  - [CPU虚拟化](#cpu虚拟化)
    - [Intel VT-x (VMX) 技术](#intel-vt-x-vmx-技术)
    - [VMExit处理详解](#vmexit处理详解)
    - [AMD-V (SVM) 技术](#amd-v-svm-技术)
  - [内存虚拟化](#内存虚拟化)
    - [EPT (Extended Page Table)](#ept-extended-page-table)
    - [KVM MMU实现](#kvm-mmu实现)
  - [I/O虚拟化](#io虚拟化)
    - [VirtIO架构](#virtio架构)
    - [Vhost加速](#vhost加速)
  - [中断虚拟化](#中断虚拟化)
    - [APIC虚拟化](#apic虚拟化)
  - [嵌套虚拟化](#嵌套虚拟化)
    - [嵌套KVM实现](#嵌套kvm实现)
  - [热迁移与快照](#热迁移与快照)
    - [热迁移实现](#热迁移实现)
  - [性能优化技术](#性能优化技术)
    - [CPU优化](#cpu优化)
    - [内存优化](#内存优化)
    - [I/O优化](#io优化)
  - [安全与隔离](#安全与隔离)
    - [机密计算](#机密计算)
  - [实践指南](#实践指南)
    - [KVM API使用示例](#kvm-api使用示例)
  - [层次关联与映射分析](#层次关联与映射分析)
  - [决策矩阵](#决策矩阵)
    - [I/O虚拟化选择决策树](#io虚拟化选择决策树)
    - [内存配置决策表](#内存配置决策表)
  - [相关资源](#相关资源)
    - [官方文档](#官方文档)
    - [书籍](#书籍)
    - [论文](#论文)
    - [工具](#工具)
  - [质量验收清单](#质量验收清单)

---

## 本节概要

| 属性 | 内容 |
|------|------|
| **核心主题** | KVM虚拟化原理、实现机制与优化技术 |
| **硬件支持** | Intel VT-x (VMX), AMD-V (SVM), ARM64 VHE |
| **关键组件** | KVM内核模块, QEMU用户态, virtio, vhost |
| **虚拟化类型** | 全虚拟化, 硬件辅助虚拟化, 准虚拟化 |
| **性能目标** | 接近原生性能 (<5% 开销) |
| **适用场景** | 云服务器, 容器基础设施, 开发测试环境 |

---

## 虚拟化技术概述

### 虚拟化技术演进

```
┌─────────────────────────────────────────────────────────────────┐
│                    虚拟化技术演进历程                           │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  1. 全虚拟化 (2001)        VMware ESX, 二进制翻译              │
│     特点: 无需修改Guest OS，性能开销大 (~30-40%)               │
│                                                                 │
│  2. 准虚拟化 (2003)        Xen, 修改Guest OS内核               │
│     特点: 特权指令陷入Hypercall，性能提升 (~10-20%)            │
│                                                                 │
│  3. 硬件辅助虚拟化 (2005)  Intel VT-x, AMD-V                   │
│     特点: CPU原生支持，接近原生性能 (<5%)                      │
│                                                                 │
│  4. 轻量级虚拟化 (2013)    KVM + virtio, 容器(LXC/Docker)      │
│     特点: 内核级虚拟化，共享宿主机内核                         │
│                                                                 │
│  5. 现代KVM (2024)         嵌套虚拟化, 机密计算(TDX,SEV-SNP)   │
│     特点: 云原生优化, 零信任安全, DPU卸载                      │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 虚拟化技术对比

| 特性 | 全虚拟化 | 准虚拟化 | 硬件辅助 | 容器 |
|------|----------|----------|----------|------|
| **Guest修改** | 无需 | 需要 | 无需 | 共享内核 |
| **性能** | ~60-70% | ~80-90% | ~95-99% | ~99%+ |
| **隔离性** | 强 | 强 | 强 | 中 |
| **启动时间** | 分钟级 | 分钟级 | 秒级 | 毫秒级 |
| **密度** | 低 | 中 | 高 | 极高 |
| **适用** | 遗留系统 | 特定OS | 通用云 | 微服务 |

---

## KVM架构与核心组件

### KVM整体架构

```
┌─────────────────────────────────────────────────────────────────────────┐
│                            KVM架构全景                                  │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                        User Space                               │   │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────────────┐    │   │
│  │  │  QEMU   │  │  libvirt│  │  VirtIO │  │  VFIO (PCI直通) │    │   │
│  │  │  主进程 │  │  管理   │  │  设备   │  │  设备直通       │    │   │
│  │  └────┬────┘  └────┬────┘  └────┬────┘  └────────┬────────┘    │   │
│  │       └─────────────┴─────────────┴────────────────┘              │   │
│  │                           │                                      │   │
│  │                     /dev/kvm  (ioctls)                           │   │
│  └───────────────────────────┼──────────────────────────────────────┘   │
│                              ↓                                          │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                      Kernel Space                               │   │
│  │  ┌─────────────────────────────────────────────────────────┐    │   │
│  │  │                 KVM Kernel Module                       │    │   │
│  │  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐    │    │   │
│  │  │  │ VMX/SVM │  │  MMU   │  │  IRQ    │  │ vCPU    │    │    │   │
│  │  │  │ CPU虚拟化│  │内存管理 │  │ 中断    │  │ 调度    │    │    │   │
│  │  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘    │    │   │
│  │  │  ┌─────────┐  ┌─────────┐  ┌─────────┐                 │    │   │
│  │  │  │  EPT   │  │  IOMMU  │  │  APICv  │                 │    │   │
│  │  │  │ 影子页表│  │ 直通    │  │ 虚拟中断│                 │    │   │
│  │  │  └─────────┘  └─────────┘  └─────────┘                 │    │   │
│  │  └─────────────────────────────────────────────────────────┘    │   │
│  │                           │                                      │   │
│  │                     Hardware Abstraction                        │   │
│  └───────────────────────────┼──────────────────────────────────────┘   │
│                              ↓                                          │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                      Hardware Layer                             │   │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐            │   │
│  │  │  CPU    │  │  MMU   │  │  VT-d   │  │ SR-IOV  │            │   │
│  │  │ VMX/SVM │  │ EPT/NPT│  │ IOMMU   │  │ 网卡    │            │   │
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘            │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 核心组件职责

| 组件 | 位置 | 职责 |
|------|------|------|
| **QEMU** | 用户态 | 设备模拟、VM生命周期管理、I/O处理 |
| **KVM** | 内核态 | CPU/内存虚拟化、VMExit处理、中断注入 |
| **libvirt** | 用户态 | 虚拟机管理API、网络/存储管理 |
| **virtio** | 内核+用户态 | 准虚拟化设备驱动 |
| **vhost** | 内核态 | 加速virtio网络/块设备 |
| **VFIO** | 内核态 | PCI设备直通、IOMMU管理 |

---

## CPU虚拟化

### Intel VT-x (VMX) 技术

```c
/**
 * Intel VT-x 核心概念
 *
 * VMX操作模式:
 * - VMX Root Mode: Hypervisor运行 (Ring 0)
 * - VMX Non-Root Mode: Guest运行 (Ring 0/1/2/3)
 *
 * VMEntry: Root -> Non-Root (进入Guest)
 * VMExit: Non-Root -> Root (退出到Hypervisor)
 */

// VMX核心数据结构
struct vmcs {
    // Guest State Area: 保存Guest CPU状态
    // 包括: RIP, RSP, RFLAGS, CR0-4, Segment Registers等

    // Host State Area: 保存Host CPU状态
    // VMExit时自动恢复

    // VM Execution Control: 控制Guest执行
    // Pin-based, Primary/Secondary Processor-based

    // VM Exit Control: 控制VMExit行为

    // VM Entry Control: 控制VMEntry行为

    // VM Exit Information: VMExit原因和详情
};

// VMX操作基本流程 (伪代码)
void vmx_operation_flow(void) {
    // 1. 检测VMX支持
    if (!cpu_has_vmx()) {
        panic("VMX not supported");
    }

    // 2. 启用VMX
    cr4 |= CR4_VMXE;  // 设置CR4.VMXE位

    // 3. 分配VMXON区域并初始化
    vmxon_region = alloc_page();
    vmxon(vmxon_region);  // 进入VMX Root Mode

    // 4. 创建VM
    vmcs_region = alloc_page();
    vmclear(vmcs_region);
    vmptrld(vmcs_region);

    // 5. 配置VMCS
    setup_guest_state();
    setup_host_state();
    setup_execution_controls();
    setup_exit_controls();

    // 6. 启动Guest
    vmlaunch();  // 首次进入
    // ... Guest运行 ...

    // 7. VMExit处理循环
    while (1) {
        vmresume();  // 恢复Guest

        // VMExit发生，处理退出原因
        exit_reason = vmread(VM_EXIT_REASON);

        switch (exit_reason) {
            case EXIT_REASON_CPUID:
                handle_cpuid();
                break;
            case EXIT_REASON_HLT:
                handle_hlt();
                break;
            case EXIT_REASON_IO_INSTRUCTION:
                handle_io();
                break;
            case EXIT_REASON_EPT_VIOLATION:
                handle_ept_violation();
                break;
            case EXIT_REASON_EXTERNAL_INTERRUPT:
                handle_external_interrupt();
                break;
            // ... 更多退出原因
        }
    }

    // 8. 关闭VMX
    vmxoff();
}
```

### VMExit处理详解

```c
/**
 * VMExit处理是KVM的核心
 * 优化目标是减少VMExit频率和开销
 */

// VMExit原因分类
#define VMEXIT_REASONS \
    X(EXCP_NMI,           0,  "异常/NMI") \
    X(EXTERNAL_INT,       1,  "外部中断") \
    X(TRIPLE_FAULT,       2,  "三重故障") \
    X(INTERRUPT_WINDOW,   7,  "中断窗口") \
    X(CPUID,             10,  "CPUID指令") \
    X(HLT,               12,  "HLT指令") \
    X(INVD,              13,  "INVD指令") \
    X(INVLPG,            14,  "INVLPG指令") \
    X(RDPMC,             15,  "RDPMC指令") \
    X(RDTSC,             16,  "RDTSC指令") \
    X(VMCALL,            18,  "VMCALL指令") \
    X(VMCLEAR,           19,  "VMCLEAR指令") \
    X(VMLAUNCH,          20,  "VMLAUNCH指令") \
    X(VMPTRLD,           21,  "VMPTRLD指令") \
    X(READ_CR3,          28,  "读CR3") \
    X(WRITE_CR3,         29,  "写CR3") \
    X(READ_CR8,          32,  "读CR8") \
    X(WRITE_CR8,         33,  "写CR8") \
    X(VMENTRY_GUEST,     55,  "VMEntry失败") \
    X(EPT_VIOLATION,     48,  "EPT违规") \
    X(EPT_MISCONFIG,     49,  "EPT配置错误") \
    X(PAUSE,             64,  "PAUSE指令") \
    X(WBINVD,            54,  "WBINVD指令")

// KVM VMExit处理函数示例 (简化)
static int handle_cpuid(struct kvm_vcpu *vcpu) {
    u32 eax, ebx, ecx, edx;

    // 读取Guest传入的参数
    eax = kvm_rax_read(vcpu);
    ecx = kvm_rcx_read(vcpu);

    // 处理CPUID
    switch (eax) {
        case 0:  // 厂商信息
            // 返回 "KVMKVMKVM\0\0\0"
            eax = 0x40000000;
            ebx = 0x4b4d564b;  // "KVMK"
            ecx = 0x564b4d56;  // "VMKV"
            edx = 0x4d564b4d;  // "MKVM"
            break;

        case 1:  // 处理器特征
            // 获取真实CPU特征
            cpuid_count(eax, ecx, &eax, &ebx, &ecx, &edx);

            // 隐藏某些特征 (如Hypervisor位)
            ecx &= ~BIT(31);  // 清除Hypervisor位

            // 根据KVM能力添加/移除特征
            if (kvm_cpu_cap_has(X86_FEATURE_X2APIC))
                ecx |= BIT(X86_FEATURE_X2APIC);
            break;

        case 0x40000001:  // KVM特征叶
            eax = KVM_FEATURE_CLOCKSOURCE |
                  KVM_FEATURE_NOP_IO_DELAY |
                  KVM_FEATURE_CLOCKSOURCE2 |
                  KVM_FEATURE_ASYNC_PF |
                  KVM_FEATURE_STEAL_TIME |
                  KVM_FEATURE_PV_EOI |
                  KVM_FEATURE_PV_UNHALT;
            ebx = ecx = edx = 0;
            break;

        default:
            // 透传给物理CPUID
            cpuid_count(eax, ecx, &eax, &ebx, &ecx, &edx);
    }

    // 写回结果
    kvm_rax_write(vcpu, eax);
    kvm_rbx_write(vcpu, ebx);
    kvm_rcx_write(vcpu, ecx);
    kvm_rdx_write(vcpu, edx);

    return kvm_skip_emulated_instruction(vcpu);
}
```

### AMD-V (SVM) 技术

```c
/**
 * AMD-V (Secure Virtual Machine) 技术
 *
 * 与Intel VT-x类似，但术语不同:
 * - VMX Root/Non-Root -> Host/Guest Mode
 * - VMCS -> VMCB (Virtual Machine Control Block)
 * - VMEntry/VMExit -> VMRUN/#VMEXIT
 */

struct vmcb {
    // Control Area
    struct {
        u32 intercept_cr;       // CR访问拦截
        u32 intercept_dr;       // DR访问拦截
        u32 intercept_exceptions; // 异常拦截
        u64 intercept;          // 指令拦截

        u8 reserved_1[44];

        u16 pause_filter_threshold;  // PAUSE过滤阈值
        u16 pause_filter_count;      // PAUSE过滤计数
        u64 iopm_base_pa;       // I/O权限表基址
        u64 msrpm_base_pa;      // MSR权限表基址

        u64 tsc_offset;         // TSC偏移
        u32 asid;               // ASID (Address Space ID)

        // ... 更多控制字段
    } control;

    // Save State Area
    struct {
        // 段寄存器, CR, RIP, RSP等
        // 与VMX Guest State Area类似
    } save;
} __packed;

// SVM关键特性: ASID (Address Space ID)
// 避免每次VM切换时刷新TLB
void svm_asid_management(void) {
    // 每个VM分配唯一ASID
    // VM切换时只需切换ASID，无需刷新TLB

    struct vcpu_svm *svm = to_svm(vcpu);

    // 检查是否需要新ASID (TLB已用ASID耗尽)
    if (kvm_vcpu_apicv_active(vcpu))
        svm->vmcb->control.virt_ext |= AVIC_ENABLE;

    // ASID管理由硬件自动完成
    svm->vmcb->control.asid = svm->asid;
}
```

---

## 内存虚拟化

### EPT (Extended Page Table)

```c
/**
 * Intel EPT (Extended Page Table) 技术
 *
 * 硬件辅助的二级地址转换:
 * - Guest虚拟地址 (GVA) -> Guest物理地址 (GPA) -> Host物理地址 (HPA)
 * - 通过EPT页表完成GPA到HPA的转换
 */

// EPT页表结构 (4级页表)
#define EPT_LEVELS 4
#define EPT_PGDIR_SHIFT 39
#define EPT_PMD_SHIFT 30
#define EPT_PTE_SHIFT 21
#define EPT_PAGE_SHIFT 12

// EPT页表项标志位
#define EPT_READABLE    (1ULL << 0)
#define EPT_WRITABLE    (1ULL << 1)
#define EPT_EXECUTABLE  (1ULL << 2)
#define EPT_EMT_SHIFT   3       // EPT内存类型
#define EPT_IGNORE_PAT  (1ULL << 6)
#define EPT_LARGE_PAGE  (1ULL << 7)  // 大页指示
#define EPT_ACCESSED    (1ULL << 8)
#define EPT_DIRTY       (1ULL << 9)
#define EPT_EXECUTE_USER (1ULL << 10)  // XD for user mode

// EPT违规处理
static int handle_ept_violation(struct kvm_vcpu *vcpu) {
    gpa_t gpa;           // Guest物理地址
    u64 exit_qualification;
    int exit_reason;

    // 获取引发违规的GPA
    gpa = vmcs_read64(GUEST_PHYSICAL_ADDRESS);
    exit_qualification = vmcs_readl(EXIT_QUALIFICATION);

    // 解析退出资格
    bool is_write = exit_qualification & (1ULL << 1);
    bool is_exec = exit_qualification & (1ULL << 2);
    bool is_user = exit_qualification & (1ULL << 7);

    // 检查是否是MMIO
    if (is_mmio_range(gpa)) {
        // MMIO访问 - 需要模拟
        return kvm_handle_mmio(vcpu, gpa, is_write);
    }

    // 正常的内存访问，需要建立EPT映射
    // 1. 分配Host物理页
    struct page *page = alloc_page(GFP_KERNEL);
    if (!page)
        return -ENOMEM;

    hpa_t hpa = page_to_phys(page);

    // 2. 建立EPT页表映射
    int ret = kvm_mmu_map_gpa(vcpu, gpa, hpa,
                              is_write ? EPT_WRITABLE : 0 |
                              is_exec ? EPT_EXECUTABLE : 0);

    return ret;
}

// EPT页表遍历 (软件模拟)
static int ept_walk(struct kvm_vcpu *vcpu, gpa_t gpa, hpa_t *hpa) {
    struct ept_pointer eptp;
    u64 *ept_entry;
    int level;

    // 获取EPT指针
    eptp.val = vmcs_read64(EPT_POINTER);

    // EPT根页表物理地址
    hpa_t ept_root = eptp.base_address;

    // 逐级遍历
    gpa_t index = gpa;

    for (level = EPT_LEVELS - 1; level >= 0; level--) {
        int shift = EPT_PAGE_SHIFT + level * 9;
        int offset = (index >> shift) & 0x1FF;

        // 获取当前级页表项
        ept_entry = phys_to_virt(ept_root) + offset * sizeof(u64);

        if (!(*ept_entry & EPT_READABLE)) {
            // 页表项不存在 - EPT违规
            return -EFAULT;
        }

        if (level == 0 || (*ept_entry & EPT_LARGE_PAGE)) {
            // 找到映射
            int page_shift = level == 0 ? EPT_PAGE_SHIFT :
                             (level == 1 ? EPT_PTE_SHIFT : EPT_PMD_SHIFT);
            u64 page_mask = ~((1ULL << page_shift) - 1);

            *hpa = (*ept_entry & page_mask) | (gpa & ~page_mask);
            return 0;
        }

        // 进入下一级
        ept_root = *ept_entry & PHYSICAL_PAGE_MASK;
    }

    return -EINVAL;
}
```

### KVM MMU实现

```c
/**
 * KVM MMU管理
 *
 * 管理GVA->GPA->HPA的完整映射
 * 包括影子页表(软件)和EPT(硬件)两种模式
 */

// KVM页表结构
struct kvm_mmu {
    // 页表根
    hpa_t root_hpa;
    gpa_t root_cr3;     // Guest CR3值

    // 页表缓存
    struct kvm_mmu_page *prev_roots[KVM_MMU_NUM_PREV_ROOTS];

    // 特性标志
    bool shadow_root_allocated;
    bool direct_map;    // 是否直接映射 (EPT模式)

    // 页表操作函数指针
    int (*page_fault)(struct kvm_vcpu *vcpu, gpa_t gpa, u32 err);
    void (*inject_page_fault)(struct kvm_vcpu *vcpu, ...);

    // TLB管理
    u8 invlpg_counter;
    u32 mmu_seq;
};

// 页故障处理 (核心函数)
int kvm_mmu_page_fault(struct kvm_vcpu *vcpu, gpa_t cr2_or_gpa, u64 err) {
    int r;
    enum emulation_result er;

    // 1. 尝试快速路径 - 如果映射已存在只是需要更新权限
    r = kvm_mmu_reload(vcpu);
    if (unlikely(r))
        return r;

    // 2. 处理页故障
    r = vcpu->arch.mmu.page_fault(vcpu, cr2_or_gpa, lower_32_bits(err));

    if (r < 0) {
        // 错误处理
        return r;
    }

    if (!r) {
        // 成功建立映射
        return 1;  // 重试指令
    }

    // 需要模拟或注入异常
    if (is_guest protection fault) {
        // 注入#PF到Guest
        kvm_inject_page_fault(vcpu, &vcpu->arch.exception);
        return 0;
    }

    // MMIO或其他特殊情况
    er = kvm_handle_mmio_page_fault(vcpu, cr2_or_gpa, direct_access);

    return 0;
}

// 大页支持
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
// 2MB大页
#define KVM_PAGES_PER_HPAGE(x)  (1ULL << (x) - PAGE_SHIFT)

static int kvm_mmu_hugepage_adjust(struct kvm_vcpu *vcpu, gfn_t gfn,
                                   int max_level, kvm_pfn_t *pfnp) {
    // 检查是否可以使用大页
    if (!kvm->arch.tdp_mmu_enabled)
        return PG_LEVEL_4K;

    // 检查Host是否支持THP
    struct page *page = pfn_to_page(*pfnp);
    if (PageTransCompound(page)) {
        // 使用2MB大页
        return PG_LEVEL_2M;
    }

    return PG_LEVEL_4K;
}
#endif
```

---

## I/O虚拟化

### VirtIO架构

```c
/**
 * VirtIO - 准虚拟化I/O标准
 *
 * 组成:
 * - 前端驱动: Guest内核中的驱动
 * - 传输层: VirtQueue
 * - 后端设备: QEMU或vhost实现
 */

// VirtQueue结构
struct virtqueue {
    struct list_head list;
    void (*callback)(struct virtqueue *vq);
    const char *name;
    struct virtio_device *vdev;

    // 虚拟队列实现
    struct vring vring;           // vring数据结构
    u16 free_head;                // 空闲描述符头
    u16 num_added;                // 本次添加的描述符数

    // 通知控制
    bool indirect;
    bool event;

    // 私有数据
    void *priv;
};

// VirtQueue操作
struct virtqueue *virtqueue_create(struct virtio_device *vdev,
                                   unsigned int index,
                                   unsigned int num,
                                   void (*callback)(struct virtqueue *),
                                   const char *name) {
    struct virtqueue *vq;

    vq = kmalloc(sizeof(*vq), GFP_KERNEL);
    if (!vq)
        return NULL;

    // 初始化vring
    vring_init(&vq->vring, num, vdev->addr + VIRTIO_PCI_QUEUE_ADDR,
               VIRTIO_PCI_VRING_ALIGN);

    vq->vdev = vdev;
    vq->index = index;
    vq->callback = callback;
    vq->name = name;

    return vq;
}

// 添加缓冲区到VirtQueue
int virtqueue_add(struct virtqueue *vq,
                  struct scatterlist *sgs[],
                  unsigned int total_out,
                  unsigned int total_in,
                  void *data) {
    struct vring_desc *desc;
    unsigned int i, head, prev;

    // 获取空闲描述符
    head = vq->free_head;
    desc = vq->vring.desc;

    // 填充描述符链
    for (i = 0; i < total_out + total_in; i++) {
        int len = sg_dma_len(sgs[i]);
        void *addr = sg_virt(sgs[i]);

        desc[head].addr = virt_to_phys(addr);
        desc[head].len = len;
        desc[head].flags = 0;

        if (i < total_out) {
            // 输出描述符
            desc[head].flags |= VRING_DESC_F_WRITE;
        }

        if (i < total_out + total_in - 1) {
            // 链接到下一个
            desc[head].flags |= VRING_DESC_F_NEXT;
            desc[head].next = prev = head;
            head = vq->free_head = desc[head].next;
        }
    }

    // 添加到可用环
    vq->vring.avail->ring[vq->vring.avail->idx % vq->vring.num] = head;
    smp_wmb();  // 内存屏障
    vq->vring.avail->idx++;

    // 通知后端 (kick)
    virtqueue_kick(vq);

    return 0;
}
```

### Vhost加速

```c
/**
 * Vhost - 内核态VirtIO后端
 *
 * 将数据面处理移到内核，避免用户态/内核态切换
 * 显著提升网络I/O性能
 */

// Vhost设备结构
struct vhost_dev {
    // 工作线程
    struct vhost_worker *worker;

    // VirtQueue
    struct vhost_virtqueue vqs[VHOST_MAX_VRINGS];
    int nvqs;

    // 内存映射
    struct vhost_umem *umem;
    struct vhost_umem *iotlb;

    // 特性协商
    u64 features;
    u64 acked_features;

    // 锁
    struct mutex mutex;
};

// Vhost网络处理 (内核态)
static int vhost_net_rx_peek_head_len(struct vhost_net *net,
                                      struct sock *sk) {
    struct vhost_virtqueue *vq = &net->dev.vqs[VHOST_NET_VQ_RX];

    // 检查是否有可用描述符
    if (!vhost_vq_avail_empty(&net->dev, vq)) {
        // 直接在内核处理数据包
        struct sk_buff *skb = skb_peek(&sk->sk_receive_queue);
        if (skb)
            return skb->len;
    }

    return 0;
}

// Vhost数据包处理流程
void vhost_net_process_packets(struct vhost_net *net) {
    struct vhost_virtqueue *rvq = &net->dev.vqs[VHOST_NET_VQ_RX];
    struct vhost_virtqueue *tvq = &net->dev.vqs[VHOST_NET_VQ_TX];

    // 处理TX (Guest -> Host)
    for (;;) {
        // 检查可用描述符
        if (vhost_vq_avail_empty(&net->dev, tvq))
            break;

        // 获取Guest发送的数据包
        head = vhost_get_vq_desc(&net->dev, tvq, vq->iov,
                                 ARRAY_SIZE(vq->iov), &out, &in,
                                 NULL, NULL);

        // 直接发送到网络栈 (不经过用户态QEMU)
        len = vhost_net_tx_packet(net, vq->iov, out);

        // 归还描述符
        vhost_add_used_and_signal(&net->dev, tvq, head, 0);
    }

    // 处理RX (Host -> Guest)
    for (;;) {
        // 检查是否有网络包和可用描述符
        if (!vhost_vq_avail_empty(&net->dev, rvq) &&
            vhost_net_rx_peek_head_len(net, sock)) {

            // 直接接收数据包到Guest内存
            head = vhost_get_vq_desc(&net->dev, rvq, vq->iov, ...);
            len = vhost_net_rx_packet(net, vq->iov, in);

            vhost_add_used_and_signal(&net->dev, rvq, head, len);
        }
    }
}
```

---

## 中断虚拟化

### APIC虚拟化

```c
/**
 * APIC (Advanced Programmable Interrupt Controller) 虚拟化
 *
 * 包括:
 * - Virtual APIC: 软件模拟APIC页面
 * - APICv: 硬件辅助APIC虚拟化 (Intel)
 * - AVIC: AMD Virtual Interrupt Controller
 * - Posted Interrupts: 中断直接投递到Guest
 */

// Virtual APIC页面结构
struct kvm_lapic {
    struct kvm_vcpu *vcpu;
    void *regs;           // APIC寄存器页面 (4KB)

    // APIC状态
    bool sw_enabled;      // 软件启用
    bool hw_enabled;      // 硬件启用 (x2APIC)

    // 定时器
    struct hrtimer timer;
    u32 timer_divisor;

    // 中断状态
    int pending_events;
};

// APICv支持检查
bool kvm_vcpu_apicv_active(struct kvm_vcpu *vcpu) {
    // 检查CPU支持
    if (!cpu_has_vmx_apicv())
        return false;

    // 检查KVM配置
    if (!kvm->arch.apicv_inhibit_reasons)
        return true;

    return false;
}

// 中断注入到Guest
int kvm_apic_set_irq(struct kvm_vcpu *vcpu, struct kvm_lapic_irq *irq) {
    struct kvm_lapic *apic = vcpu->arch.apic;

    // 使用APICv加速
    if (kvm_vcpu_apicv_active(vcpu) &&
        !kvm_x86_ops.sync_pir_to_irr(vcpu)) {
        // 硬件直接处理Posted Interrupt
        return 0;
    }

    // 软件模拟路径
    u32 vector = irq->vector;

    // 设置IRR (Interrupt Request Register)
    kvm_apic_set_irr(vector, apic);

    // 请求中断注入
    kvm_make_request(KVM_REQ_EVENT, vcpu);
    kvm_vcpu_kick(vcpu);

    return 0;
}

// Posted Interrupt处理 (Intel VT-x)
void vmx_update_pi_irte(struct kvm *kvm, unsigned int host_irq,
                        uint32_t guest_irq, bool set) {
    struct kvm_kernel_irq_routing_entry *e;
    struct kvm_irq_routing_table *irq_rt;

    // 更新Posted Interrupt Remapping Table Entry
    // 允许外部中断直接投递到目标vCPU，无需VMExit

    irq_rt = rcu_dereference(kvm->irq_routing);

    for (i = 0; i < irq_rt->nr_rt_entries; i++) {
        e = &irq_rt->rt_entries[i];

        if (e->type == KVM_IRQ_ROUTING_IRQCHIP &&
            e->irqchip.irq == guest_irq) {
            // 配置IRTE指向目标vCPU的Posted Interrupt Descriptor
            irq_set_vcpu_affinity(host_irq, vcpu);
        }
    }
}
```

---

## 嵌套虚拟化

### 嵌套KVM实现

```c
/**
 * 嵌套虚拟化 (Nested Virtualization)
 *
 * L0: 最底层Hypervisor (KVM)
 * L1: 嵌套Guest (Guest中的Hypervisor)
 * L2: 嵌套Guest的Guest
 *
 * 实现方式: VMX Shadowing / NPT Shadowing
 */

// 嵌套VMCS管理
struct vmcs12 {
    // L1 hypervisor设置的VMCS字段
    // 由L0保存和恢复

    u32 revision_id;
    u32 abort_indicator;

    // Guest State Area
    u64 guest_rip;
    u64 guest_rsp;
    u64 guest_rflags;
    u64 guest_cr0, guest_cr3, guest_cr4;
    // ...

    // Host State Area
    u64 host_rip;
    u64 host_rsp;
    u64 host_cr0, host_cr3, host_cr4;
    // ...

    // Control Fields
    u32 pin_based_vm_exec_control;
    u32 cpu_based_vm_exec_control;
    u32 exception_bitmap;
    u64 ept_pointer;
    // ...
};

// VMEntry到L2 (嵌套Guest)
int nested_vmx_enter_non_root_mode(struct kvm_vcpu *vcpu, bool from_vmentry) {
    struct vcpu_vmx *vmx = to_vmx(vcpu);

    // 检查VMCS12有效性
    if (!check_vmentry_prereqs(vcpu) ||
        !check_vmentry_postreqs(vcpu, vmcs12)) {
        return 1;  // VMEntry失败
    }

    // 进入嵌套模式
    vmx->nested.nested_run_pending = 1;

    // 加载VMCS12到VMCS02 (由KVM准备的VMCS)
    // VMCS02合并L0和L1的控制设置

    // 保存L1状态
    vmx->nested.vmcs01_guest_rip = vmcs_readl(GUEST_RIP);
    vmx->nested.vmcs01_guest_rsp = vmcs_readl(GUEST_RSP);

    // 设置L2 Guest状态
    vmcs_writel(GUEST_RIP, vmcs12->guest_rip);
    vmcs_writel(GUEST_RSP, vmcs12->guest_rsp);
    vmcs_writel(GUEST_CR3, vmcs12->guest_cr3);

    // 配置EPT (如果启用)
    if (nested_cpu_has_ept(vmcs12)) {
        // 使用合并的EPT页表 (EPT12指向L2的GPA->GPA映射)
        vmcs_writel(EPT_POINTER, construct_eptp(&vmx->nested.ept12));
    }

    // 执行VMLAUNCH/VMRESUME进入L2
    vmx->fail = __vmx_vcpu_run(vmx, vmx->loaded_vmcs->launched);

    return 0;
}

// VMExit从L2处理
int nested_vmx_vmexit(struct kvm_vcpu *vcpu, u32 exit_reason,
                      u32 exit_intr_info, unsigned long exit_qualification) {
    struct vcpu_vmx *vmx = to_vmx(vcpu);
    struct vmcs12 *vmcs12 = get_vmcs12(vcpu);

    // 判断是否需要转发到L1
    if (nested_vmx_exit_reflected(vcpu, exit_reason)) {
        // 保存L2退出信息到VMCS12
        vmcs12->vm_exit_reason = exit_reason;
        vmcs12->vm_exit_intr_info = exit_intr_info;
        vmcs12->vm_exit_qualification = exit_qualification;
        vmcs12->guest_rip = vmcs_readl(GUEST_RIP);
        vmcs12->guest_rsp = vmcs_readl(GUEST_RSP);

        // 切换到L1处理VMExit
        vmcs_writel(GUEST_RIP, vmcs12->host_rip);
        vmcs_writel(GUEST_RSP, vmcs12->host_rsp);

        // 注入VIRTUALIZATION_EXCEPTION到L1 (如果启用)
        if (nested_cpu_has_vmfunc(vmcs12) &&
            (exit_reason == EXIT_REASON_EPT_VIOLATION ||
             exit_reason == EXIT_REASON_EPT_MISCONFIG)) {
            kvm_queue_exception_e(vcpu, VE_VECTOR, 0);
        }
    } else {
        // L0处理VMExit
        handle_vmexit(vcpu, exit_reason, exit_intr_info, exit_qualification);
    }

    return 0;
}
```

---

## 热迁移与快照

### 热迁移实现

```c
/**
 * 热迁移 (Live Migration)
 *
 * 流程:
 * 1. 预复制 (Pre-copy): 迭代传输内存页
 * 2. 停机复制 (Stop-and-copy): 传输脏页和状态
 * 3. 恢复 (Resume): 目标端恢复执行
 */

// 脏页追踪
struct kvm_memory_slot {
    gfn_t base_gfn;
    unsigned long npages;

    // 脏页追踪
    unsigned long *dirty_bitmap;
    unsigned long *dirty_bitmap_head;

    // 写保护
    bool write_protect;
};

// 启用脏页追踪
int kvm_vm_ioctl_get_dirty_log(struct kvm *kvm,
                                struct kvm_dirty_log *log) {
    struct kvm_memory_slot *memslot;
    unsigned long n;
    unsigned long *dirty_bitmap;

    memslot = id_to_memslot(kvm->memslots, log->slot);

    // 获取脏页位图
    dirty_bitmap = memslot->dirty_bitmap;

    // 将脏页位图复制到用户空间
    n = kvm_dirty_bitmap_bytes(memslot);
    if (copy_to_user(log->dirty_bitmap, dirty_bitmap, n))
        return -EFAULT;

    // 重置脏页位图 (如果请求)
    if (log->flags & KVM_DIRTY_LOG_INITIALLY_SET) {
        memset(dirty_bitmap, 0, n);

        // 重新启用写保护或EPT脏页追踪
        kvm_mmu_slot_remove_write_access(kvm, memslot);
    }

    return 0;
}

// 迁移迭代
int migrate_iteration(struct kvm *kvm, struct migration_state *state) {
    int iteration = 0;
    bool last_iteration = false;

    while (!last_iteration) {
        // 获取脏页
        kvm_get_dirty_log(kvm, &dirty_log);

        // 传输脏页
        for_each_set_bit(page, dirty_log.dirty_bitmap, total_pages) {
            gfn_t gfn = memslot->base_gfn + page;
            void *page_addr = gfn_to_hva(kvm, gfn);

            // 压缩并发送页面
            send_page(state->socket, page_addr, PAGE_SIZE);
        }

        // 检查是否进入最后迭代
        if (dirty_page_count < threshold || iteration > max_iterations) {
            // 暂停VM
            kvm_pause_vcpu(kvm);

            // 最后复制
            kvm_get_dirty_log(kvm, &dirty_log);
            for_each_set_bit(page, dirty_log.dirty_bitmap, total_pages) {
                send_last_pages(...);
            }

            // 传输vCPU状态
            for (i = 0; i < kvm->nr_vcpus; i++) {
                struct kvm_vcpu *vcpu = kvm->vcpus[i];
                save_vcpu_state(vcpu, &state->vcpu_states[i]);
            }

            last_iteration = true;
        }

        iteration++;
    }

    return 0;
}
```

---

## 性能优化技术

### CPU优化

```c
/**
 * CPU虚拟化优化技术
 */

// 1. 减少VMExit
void reduce_vmexit_optimization(void) {
    // 启用MSR位图 - 避免MSR读写VMExit
    vmcs_write64(MSR_BITMAP, __pa(vmx_msr_bitmap));

    // 启用I/O位图 - 允许安全I/O端口直接访问
    vmcs_write64(IO_BITMAP_A, __pa(vmx_io_bitmap_a));
    vmcs_write64(IO_BITMAP_B, __pa(vmx_io_bitmap_b));

    // 启用APICv - 减少中断相关VMExit
    if (cpu_has_apicv()) {
        secondary_exec_controls |= SECONDARY_EXEC_APIC_REGISTER_VIRT |
                                   SECONDARY_EXEC_VIRTUAL_INTR_DELIVERY;
    }

    // PAUSE循环退出 - 优化自旋锁
    vmcs_write32(PAUSE_LOOP_EXITING, 1000);  // 1000次PAUSE后退出

    // 启用Posted Interrupts
    if (cpu_has_posted_intr()) {
        vmcs_write64(POSTED_INTR_NV, POSTED_INTR_VECTOR);
        vmcs_write64(POSTED_INTR_DESC_ADDR, __pa(&vmx->pi_desc));
    }
}

// 2. vCPU调度优化
void vcpu_scheduling_optimization(struct kvm_vcpu *vcpu) {
    // 设置vCPU亲和性
    cpumask_t mask;
    cpumask_set_cpu(preferred_cpu, &mask);
    sched_setaffinity(vcpu->pid, &mask);

    // 启用halt polling - 减少唤醒延迟
    vcpu->halt_poll_ns = 200000;  // 200μs轮询

    // 优先级设置
    set_user_nice(vcpu->task, -10);
}
```

### 内存优化

```c
/**
 * 内存虚拟化优化
 */

// 1. 大页支持
void memory_optimization(void) {
    // 启用THP (Transparent Huge Pages)
    // 自动将4KB页合并为2MB大页

    // EPT大页
    // 减少EPT页表遍历层级，提升TLB效率

    // 预分配大页 (hugetlbfs)
    // mount -t hugetlbfs none /dev/hugepages
    // qemu -mem-path /dev/hugepages
}

// 2. KSM (Kernel Samepage Merging)
void ksm_optimization(void) {
    // 合并相同内容的内存页
    // 特别适用于运行相同OS/应用的VM

    // 启用KSM
    echo 1 > /sys/kernel/mm/ksm/run

    // 调整扫描速度
    echo 1000 > /sys/kernel/mm/ksm/pages_to_scan
    echo 10 > /sys/kernel/mm/ksm/sleep_millisecs
}

// 3. 内存预分配
void memory_preallocation(struct kvm *kvm) {
    // 预分配并锁定内存，避免运行时分配延迟
    // qemu -mem-prealloc -mem-path /dev/hugepages

    // mlock所有内存
    mlockall(MCL_CURRENT | MCL_FUTURE);
}
```

### I/O优化

```c
/**
 * I/O虚拟化优化
 */

// 1. VirtIO多队列
void virtio_multiqueue(struct virtio_net *dev) {
    // 每个vCPU一个队列对 (TX/RX)
    int num_queues = num_online_cpus();

    dev->num_tx_queues = num_queues;
    dev->num_rx_queues = num_queues;

    // 分配队列
    for (i = 0; i < num_queues; i++) {
        dev->tx_vqs[i] = vring_create_virtqueue(i, 256, ...);
        dev->rx_vqs[i] = vring_create_virtqueue(i + num_queues, 256, ...);
    }
}

// 2. vhost-user
void vhost_user_optimization(void) {
    // 使用DPDK或VPP作为vhost后端
    // 绕过内核网络栈

    // 配置示例:
    // qemu -chardev socket,id=char0,path=/var/run/vhost.sock
    //      -netdev vhost-user,id=net0,chardev=char0
    //      -device virtio-net-pci,netdev=net0
}

// 3. VFIO设备直通
void vfio_passthrough(void) {
    // PCI设备直通给VM
    // 几乎原生性能

    // 配置步骤:
    // 1. 解绑设备从Host驱动
    // echo 0000:04:00.0 > /sys/bus/pci/devices/.../driver/unbind

    // 2. 绑定到VFIO
    // echo vfio-pci > /sys/bus/pci/devices/.../driver_override

    // 3. QEMU配置
    // -device vfio-pci,host=04:00.0
}
```

---

## 安全与隔离

### 机密计算

```c
/**
 * 机密计算 (Confidential Computing)
 *
 * Intel TDX (Trust Domain Extensions)
 * AMD SEV-SNP (Secure Encrypted Virtualization)
 * ARM CCA (Confidential Compute Architecture)
 */

// Intel TDX
#ifdef CONFIG_INTEL_TDX_HOST

struct tdx_td {
    // TDX模块管理的Trust Domain上下文
    u64 td_handle;

    // 内存加密密钥
    u64 hkid;  // Hardware Key ID

    // 测量值
    u8 tdmr[SHA384_SIZE];  // TDX Measurement Register

    // 配置
    struct tdx_td_config config;
};

// 创建TD (Trust Domain)
int tdx_td_create(struct tdx_td *td, struct kvm_tdx_init_vm *init_vm) {
    // 1. 分配HKID
    td->hkid = alloc_hkid();

    // 2. 配置SEAM模块
    seamcall(TDH_MNG_CREATE, ...);

    // 3. 添加内存页 (加密)
    for_each_memory_region(region, init_vm) {
        for (page = region->start; page < region->end; page++) {
            seamcall(TDH_MEM_PAGE_ADD, page, td->hkid);
        }
    }

    // 4. 最终化TD
    seamcall(TDH_MNG_FINALIZE, td->td_handle);

    return 0;
}

// TD进入 (SEAM模式)
int tdx_td_enter(struct tdx_td *td) {
    // 切换到SEAM模式执行TD
    seamcall(TDH_VP_ENTER, td->td_handle);

    // TD在加密环境中运行
    // Host无法访问TD内存

    return 0;
}

#endif /* CONFIG_INTEL_TDX_HOST */

// AMD SEV-SNP
#ifdef CONFIG_AMD_SEV

struct sev_guest {
    u32 handle;           // SEV固件句柄
    u32 policy;           // 安全策略

    // 密钥管理
    struct sev_guest_key *key;

    // 内存状态
    unsigned long pages_locked;
};

// SEV-SNP启动流程
int sev_snp_launch(struct kvm *kvm, struct sev_guest *guest) {
    // 1. 启动SEV-SNP会话
    sev_issue_cmd(SEV_CMD_SNP_GCTX_CREATE, &gctx_create);

    // 2. 分配加密内存
    for_each_memory_slot(slot, kvm) {
        sev_issue_cmd(SEV_CMD_SNP_PAGE_RECLAIM, &reclaim);
        sev_issue_cmd(SEV_CMD_SNP_GCTX_UPDATE, &update);
    }

    // 3. 加载初始代码 (已签名)
    sev_issue_cmd(SEV_CMD_SNP_LAUNCH_UPDATE, &launch_update);

    // 4. 最终化并获取证明报告
    sev_issue_cmd(SEV_CMD_SNP_GUEST_FINISH, &finish);

    return 0;
}

#endif /* CONFIG_AMD_SEV */
```

---

## 实践指南

### KVM API使用示例

```c
#include <linux/kvm.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * 最小KVM VM创建示例
 */

#define KVM_DEVICE "/dev/kvm"
#define MEMORY_SIZE (1ULL << 30)  // 1GB
#define CODE_START 0x1000

struct kvm_vm {
    int kvm_fd;
    int vm_fd;
    int vcpu_fd;
    void *memory;
    size_t memory_size;
};

// 创建KVM VM
struct kvm_vm *kvm_vm_create(void) {
    struct kvm_vm *vm = calloc(1, sizeof(*vm));

    // 打开KVM设备
    vm->kvm_fd = open(KVM_DEVICE, O_RDWR);
    if (vm->kvm_fd < 0) {
        perror("open /dev/kvm");
        return NULL;
    }

    // 检查KVM版本
    int kvm_version = ioctl(vm->kvm_fd, KVM_GET_API_VERSION, 0);
    if (kvm_version != 12) {
        fprintf(stderr, "KVM API version mismatch: %d\n", kvm_version);
        return NULL;
    }

    // 创建VM
    vm->vm_fd = ioctl(vm->kvm_fd, KVM_CREATE_VM, 0);
    if (vm->vm_fd < 0) {
        perror("KVM_CREATE_VM");
        return NULL;
    }

    // 分配内存
    vm->memory_size = MEMORY_SIZE;
    posix_memalign(&vm->memory, 0x1000, vm->memory_size);
    memset(vm->memory, 0, vm->memory_size);

    // 设置用户内存区域
    struct kvm_userspace_memory_region mem = {
        .slot = 0,
        .flags = 0,
        .guest_phys_addr = 0,
        .memory_size = vm->memory_size,
        .userspace_addr = (uint64_t)vm->memory,
    };

    if (ioctl(vm->vm_fd, KVM_SET_USER_MEMORY_REGION, &mem) < 0) {
        perror("KVM_SET_USER_MEMORY_REGION");
        return NULL;
    }

    // 创建vCPU
    vm->vcpu_fd = ioctl(vm->vm_fd, KVM_CREATE_VCPU, 0);
    if (vm->vcpu_fd < 0) {
        perror("KVM_CREATE_VCPU");
        return NULL;
    }

    return vm;
}

// 运行VM
int kvm_vm_run(struct kvm_vm *vm) {
    // 获取KVM运行区域大小
    int mmap_size = ioctl(vm->kvm_fd, KVM_GET_VCPU_MMAP_SIZE, 0);

    // 映射KVM运行区域
    struct kvm_run *run = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE,
                               MAP_SHARED, vm->vcpu_fd, 0);

    // 设置vCPU寄存器 (简化示例 - 实际应设置完整寄存器组)
    struct kvm_regs regs;
    ioctl(vm->vcpu_fd, KVM_GET_REGS, &regs);
    regs.rip = CODE_START;  // 从0x1000开始执行
    regs.rsp = MEMORY_SIZE; // 栈顶
    ioctl(vm->vcpu_fd, KVM_SET_REGS, &regs);

    // 加载简单代码 (x86 HLT指令)
    uint8_t code[] = {0xF4};  // HLT
    memcpy(vm->memory + CODE_START, code, sizeof(code));

    // 运行循环
    while (1) {
        int ret = ioctl(vm->vcpu_fd, KVM_RUN, 0);
        if (ret < 0) {
            perror("KVM_RUN");
            return -1;
        }

        // 处理退出原因
        switch (run->exit_reason) {
            case KVM_EXIT_HLT:
                printf("VM halted\n");
                return 0;

            case KVM_EXIT_IO:
                // 处理I/O
                printf("I/O exit: port=0x%x\n", run->io.port);
                break;

            case KVM_EXIT_MMIO:
                // 处理MMIO
                printf("MMIO exit\n");
                break;

            case KVM_EXIT_SHUTDOWN:
                printf("VM shutdown\n");
                return 0;

            default:
                fprintf(stderr, "Unknown exit reason: %d\n", run->exit_reason);
                return -1;
        }
    }
}

// 销毁VM
void kvm_vm_destroy(struct kvm_vm *vm) {
    if (vm->vcpu_fd >= 0) close(vm->vcpu_fd);
    if (vm->vm_fd >= 0) close(vm->vm_fd);
    if (vm->kvm_fd >= 0) close(vm->kvm_fd);
    if (vm->memory) free(vm->memory);
    free(vm);
}

// 主函数
int main(int argc, char *argv[]) {
    struct kvm_vm *vm = kvm_vm_create();
    if (!vm) {
        fprintf(stderr, "Failed to create VM\n");
        return 1;
    }

    printf("VM created successfully\n");
    printf("  KVM fd: %d\n", vm->kvm_fd);
    printf("  VM fd: %d\n", vm->vm_fd);
    printf("  VCPU fd: %d\n", vm->vcpu_fd);
    printf("  Memory: %zu MB at %p\n", vm->memory_size >> 20, vm->memory);

    // 运行VM
    kvm_vm_run(vm);

    // 清理
    kvm_vm_destroy(vm);

    return 0;
}
```

---

## 层次关联与映射分析

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        KVM虚拟化层次映射                                │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  L7 应用层         VM内部应用    Guest OS应用                          │
│       ↑                    ↓                                            │
│  L6 Guest OS层     Linux/Windows内核    设备驱动                       │
│       ↑                    ↓                                            │
│  L5 半虚拟化层     virtio驱动    paravirt接口                          │
│       ↑                    ↓                                            │
│  L4 Hypervisor层   KVM    QEMU    vhost    VFIO                        │
│       ↑                    ↓                                            │
│  L3 虚拟化硬件层   VMX/SVM    EPT/NPT    VT-d/IOMMU    APICv           │
│       ↑                    ↓                                            │
│  L2 物理硬件层     CPU核心    MMU    中断控制器    PCIe总线            │
│       ↑                    ↓                                            │
│  L1 硅片层         晶体管    微架构    硬件实现                          │
│                                                                         │
├─────────────────────────────────────────────────────────────────────────┤
│  跨层关注点: 性能监控    资源调度    安全隔离    热迁移                  │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 决策矩阵

### I/O虚拟化选择决策树

```
需要设备虚拟化?
├── 是 → 性能要求?
│       ├── 极致性能 → 设备直通?
│       │       ├── 是 → VFIO PCI直通
│       │       └── 否 → vhost-user + DPDK
│       └── 标准性能 → 兼容性要求?
│               ├── 高兼容性 → QEMU设备模拟
│               └── 标准兼容性 → VirtIO + vhost
├── 否 → 使用容器共享内核
```

### 内存配置决策表

| 场景 | 推荐配置 | 理由 |
|------|----------|------|
| 高内存带宽 | 2MB大页 + EPT大页 | 减少TLB缺失 |
| 高密度VM | KSM + 内存气球 | 内存超分配 |
| 实时性要求 | 内存预分配 + mlock | 避免页错误 |
| NUMA系统 | NUMA aware分配 | 本地内存访问 |
| 热迁移 | 脏页追踪 + 压缩 | 快速迁移 |

---

## 相关资源

### 官方文档

- [KVM Official Documentation](https://www.linux-kvm.org/page/Documents)
- [QEMU Documentation](https://qemu.readthedocs.io/)
- [Intel SDM - VMX Chapter](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [AMD APM - SVM Chapter](https://developer.amd.com/resources/developer-guides-manuals/)

### 书籍

- 《系统虚拟化：原理与实现》
- 《KVM虚拟化技术：实战与原理解析》
- 《QEMU/KVM源码解析与应用》

### 论文

- "A Comparison of Software and Hardware Techniques for x86 Virtualization" (VMware, 2006)
- "Intel Virtualization Technology for Directed I/O" (Intel, 2006)
- "AMD-V Nested Paging" (AMD, 2008)

### 工具

```bash
# KVM性能监控
perf kvm stat live

# VMExit分析
perf record -e 'kvm:*' -a sleep 10
perf script

# 查看KVM事件
trace-cmd start -e kvm
trace-cmd show
```

---

## 质量验收清单

- [ ] **CPU虚拟化**: 启用VMX/SVM，正确配置VMCS/VMCB
- [ ] **内存虚拟化**: EPT/NPT启用，大页支持配置
- [ ] **I/O虚拟化**: VirtIO设备正确配置，vhost加速启用
- [ ] **中断虚拟化**: APICv/AVIC启用，Posted Interrupts配置
- [ ] **嵌套虚拟化**: 支持运行嵌套Hypervisor
- [ ] **热迁移**: 脏页追踪启用，迁移测试通过
- [ ] **性能优化**: VMExit率<1000/秒，CPU开销<5%
- [ ] **安全隔离**: sVirt启用，SELinux策略配置
- [ ] **资源限制**: cgroups配置，防止资源耗尽
- [ ] **监控告警**: 性能指标采集，异常告警配置
- [ ] **备份恢复**: VM快照功能测试，恢复时间验证
- [ ] **高可用**: 故障自动迁移，集群配置验证

---

*文档结束*
