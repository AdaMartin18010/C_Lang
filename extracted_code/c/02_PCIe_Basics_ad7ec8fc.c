/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\10_IO_System\02_PCIe_Basics.md
 * Line: 1344
 * Language: c
 * Block ID: ad7ec8fc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux PCIe电源管理
 */

// 1. 启用ASPM
static void enable_aspm(struct pci_dev *pdev)
{
    struct pcie_link_state *link = pdev->link_state;

    // 设置ASPM策略
    if (link) {
        pcie_config_aspm_link(link, ASPM_L0S_L1);
    }
}

// 2. 挂起/恢复回调
static int my_pcie_suspend(struct device *dev)
{
    struct pci_dev *pdev = to_pci_dev(dev);
    struct my_dev *mydev = pci_get_drvdata(pdev);

    // 停止设备操作
    my_stop_device(mydev);

    // 保存设备状态
    pci_save_state(pdev);

    // 设置D3hot状态
    pci_set_power_state(pdev, PCI_D3hot);

    // 禁用设备
    pci_disable_device(pdev);

    // 设置唤醒 (如果需要)
    if (device_may_wakeup(dev))
        pci_enable_wake(pdev, PCI_D3hot, 1);

    return 0;
}

static int my_pcie_resume(struct device *dev)
{
    struct pci_dev *pdev = to_pci_dev(dev);
    struct my_dev *mydev = pci_get_drvdata(pdev);
    int err;

    // 禁用唤醒
    if (device_may_wakeup(dev))
        pci_enable_wake(pdev, PCI_D3cold, 0);

    // 重新启用设备
    err = pci_enable_device(pdev);
    if (err)
        return err;

    pci_set_master(pdev);

    // 恢复设备状态
    pci_restore_state(pdev);

    // 重新初始化设备
    my_reinit_device(mydev);

    return 0;
}

static const struct dev_pm_ops my_pcie_pm_ops = {
    .suspend = my_pcie_suspend,
    .resume = my_pcie_resume,
    .freeze = my_pcie_suspend,
    .thaw = my_pcie_resume,
    .poweroff = my_pcie_suspend,
    .restore = my_pcie_resume,
};

// 3. 运行时电源管理
static int my_pcie_runtime_suspend(struct device *dev)
{
    struct pci_dev *pdev = to_pci_dev(dev);

    // 进入D3hot
    pci_save_state(pdev);
    pci_set_power_state(pdev, PCI_D3hot);

    return 0;
}

static int my_pcie_runtime_resume(struct device *dev)
{
    struct pci_dev *pdev = to_pci_dev(dev);

    // 恢复到D0
    pci_set_power_state(pdev, PCI_D0);
    pci_restore_state(pdev);

    return 0;
}

static const struct dev_pm_ops my_pcie_runtime_pm_ops = {
    SET_RUNTIME_PM_OPS(my_pcie_runtime_suspend,
                       my_pcie_runtime_resume,
                       NULL)
};
