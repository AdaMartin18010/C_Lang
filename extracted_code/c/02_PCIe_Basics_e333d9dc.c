/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\10_IO_System\02_PCIe_Basics.md
 * Line: 671
 * Language: c
 * Block ID: e333d9dc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux内核PCI枚举核心代码
 * drivers/pci/probe.c
 */

// 1. 扫描总线上的设备
static unsigned int pci_scan_child_bus(struct pci_bus *bus)
{
    unsigned int devfn, pass, max = bus->busn_res.start;
    struct pci_dev *dev;

    dev_dbg(&bus->dev, "scanning bus\n");

    // 遍历所有设备号 (0-31) 和功能号 (0-7)
    for (devfn = 0; devfn < 0x100; devfn += 8) {
        // 尝试读取Vendor ID
        u32 l;
        if (pci_bus_read_config_dword(bus, devfn, PCI_VENDOR_ID, &l))
            continue;

        // 0xFFFFFFFF 表示设备不存在
        if (l == 0xffffffff || l == 0x00000000 ||
            l == 0x0000ffff || l == 0xffff0000)
            continue;

        // 创建设备结构并扫描
        dev = pci_scan_single_device(bus, devfn);
        if (dev)
            dev->is_added = 1;
    }

    // 对桥设备进行递归扫描
    for (pass = 0; pass < 2; pass++)
        list_for_each_entry(dev, &bus->devices, bus_list)
            if (dev->hdr_type == PCI_HEADER_TYPE_BRIDGE ||
                dev->hdr_type == PCI_HEADER_TYPE_CARDBUS)
                if (dev->subordinate)
                    pci_scan_child_bus(dev->subordinate);

    // 分配总线号范围
    max = pci_scan_bridge(bus);

    return max;
}

// 2. 扫描桥设备
static int pci_scan_bridge(struct pci_bus *bus)
{
    struct pci_dev *dev;
    int pass, max;

    max = bus->busn_res.start;

    list_for_each_entry(dev, &bus->devices, bus_list) {
        // 只处理桥设备
        if (dev->hdr_type != PCI_HEADER_TYPE_BRIDGE &&
            dev->hdr_type != PCI_HEADER_TYPE_CARDBUS)
            continue;

        // 读取桥配置
        u16 bctl;
        u8 prim, sec, sub;

        pci_read_config_byte(dev, PCI_PRIMARY_BUS, &prim);
        pci_read_config_byte(dev, PCI_SECONDARY_BUS, &sec);
        pci_read_config_byte(dev, PCI_SUBORDINATE_BUS, &sub);

        // 如果是未配置的桥
        if (sec == 0) {
            // 分配新的次级总线号
            sec = ++max;

            // 写入配置
            pci_write_config_byte(dev, PCI_SECONDARY_BUS, sec);
            pci_write_config_byte(dev, PCI_SUBORDINATE_BUS, 0xFF);

            // 创建子总线结构
            child = pci_add_new_bus(bus, dev, sec);

            // 递归扫描
            max = pci_scan_child_bus(child);

            // 更新Subordinate Bus
            pci_write_config_byte(dev, PCI_SUBORDINATE_BUS, max);
        }
    }

    return max;
}

// 3. 分配BAR资源
static void pci_setup_device(struct pci_dev *dev)
{
    u32 orig, sz;
    int i, reg;

    // 遍历所有BAR (6个64位BAR或6个32位BAR)
    for (i = 0; i < PCI_NUM_RESOURCES; i++) {
        reg = PCI_BASE_ADDRESS_0 + (i * 4);

        // 读取原始值
        pci_read_config_dword(dev, reg, &orig);

        // 写入全1来探测大小
        pci_write_config_dword(dev, reg, 0xffffffff);
        pci_read_config_dword(dev, reg, &sz);
        pci_write_config_dword(dev, reg, orig);

        // 解码大小
        if (sz == 0 || sz == 0xffffffff)
            continue;

        if (sz & PCI_BASE_ADDRESS_SPACE_IO) {
            // I/O空间
            sz &= PCI_BASE_ADDRESS_IO_MASK;
            sz = (~sz + 1) & 0xffff;
            // 分配I/O资源...
        } else {
            // 内存空间
            sz &= PCI_BASE_ADDRESS_MEM_MASK;
            sz = ~sz + 1;

            // 检查64位BAR
            if ((orig & PCI_BASE_ADDRESS_MEM_TYPE_MASK) ==
                PCI_BASE_ADDRESS_MEM_TYPE_64) {
                // 需要读取下一个BAR作为高32位
                i++;
            }
            // 分配内存资源...
        }
    }
}

// 4. 使能设备
static void pci_enable_device_resources(struct pci_dev *dev)
{
    u16 cmd, old_cmd;

    pci_read_config_word(dev, PCI_COMMAND, &cmd);
    old_cmd = cmd;

    // 使能I/O空间
    if (dev->resource[PCI_ROM_RESOURCE].flags & IORESOURCE_IO)
        cmd |= PCI_COMMAND_IO;

    // 使能内存空间
    if (dev->resource[PCI_ROM_RESOURCE].flags & IORESOURCE_MEM)
        cmd |= PCI_COMMAND_MEMORY;

    // 使能总线主控 (DMA)
    cmd |= PCI_COMMAND_MASTER;

    if (cmd != old_cmd) {
        pci_write_config_word(dev, PCI_COMMAND, cmd);
    }
}
