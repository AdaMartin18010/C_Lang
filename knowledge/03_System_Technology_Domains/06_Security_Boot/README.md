# 安全启动

> **层级定位**: 03_System_Technology_Domains > 06_Security_Boot

---

## 内容

- [ARM Trusted Firmware](./01_ARM_Trusted_Firmware.md)
- [安全启动链](./02_Secure_Boot_Chain.md)
- [可信启动](./03_Measured_Boot.md)

---

## 安全启动流程

```
Boot ROM → BL1 → BL2 → BL31 → BL32 → BL33
   ↓       ↓     ↓      ↓      ↓      ↓
 固化    信任   加载   运行时  TEE   UEFI
 代码    锚点   固件   服务    固件  /Linux
```

---

## 关键技术

- 信任链建立
- 镜像签名验证
- 安全存储
- 密钥管理

---

> **最后更新**: 2026-03-21
