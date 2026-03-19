/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 1595
 * Language: c
 * Block ID: 19bb24f9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// W25Q128 Flash驱动 (16MB SPI Flash)
// 常用于存储固件、配置、日志
// ============================================================

#define W25X_WRITE_ENABLE       0x06
#define W25X_WRITE_DISABLE      0x04
#define W25X_READ_STATUS_REG    0x05
#define W25X_READ_DATA          0x03
#define W25X_PAGE_PROGRAM       0x02
#define W25X_SECTOR_ERASE       0x20
#define W25X_CHIP_ERASE         0xC7
#define W25X_POWER_DOWN         0xB9
#define W25X_RELEASE_POWER_DOWN 0xAB
#define W25X_JEDEC_ID           0x9F

#define W25Q_PAGE_SIZE          256
#define W25Q_SECTOR_SIZE        4096
#define W25Q_BLOCK_SIZE         65536

// 读取JEDEC ID
bool w25q_read_id(SoftSPI *spi, uint32_t *id) {
    softspi_begin(spi);
    softspi_transfer(spi, W25X_JEDEC_ID);
    uint8_t manufacturer = softspi_transfer(spi, 0xFF);
    uint8_t memory_type = softspi_transfer(spi, 0xFF);
    uint8_t capacity = softspi_transfer(spi, 0xFF);
    softspi_end(spi);

    *id = (manufacturer << 16) | (memory_type << 8) | capacity;
    return (manufacturer == 0xEF);  // Winbond厂商ID
}

// 等待忙碌完成
void w25q_wait_busy(SoftSPI *spi) {
    softspi_begin(spi);
    softspi_transfer(spi, W25X_READ_STATUS_REG);
    while (softspi_transfer(spi, 0xFF) & 0x01) {
        // 忙等待
    }
    softspi_end(spi);
}

// 写使能
void w25q_write_enable(SoftSPI *spi) {
    softspi_begin(spi);
    softspi_transfer(spi, W25X_WRITE_ENABLE);
    softspi_end(spi);
}

// 擦除扇区 (4KB)
bool w25q_sector_erase(SoftSPI *spi, uint32_t addr) {
    w25q_write_enable(spi);

    softspi_begin(spi);
    softspi_transfer(spi, W25X_SECTOR_ERASE);
    softspi_transfer(spi, (addr >> 16) & 0xFF);
    softspi_transfer(spi, (addr >> 8) & 0xFF);
    softspi_transfer(spi, addr & 0xFF);
    softspi_end(spi);

    w25q_wait_busy(spi);
    return true;
}

// 页编程 (最多256字节)
bool w25q_page_program(SoftSPI *spi, uint32_t addr, const uint8_t *data, size_t len) {
    if (len > W25Q_PAGE_SIZE) return false;
    if ((addr & 0xFF) + len > W25Q_PAGE_SIZE) return false;  // 跨页检查

    w25q_write_enable(spi);

    softspi_begin(spi);
    softspi_transfer(spi, W25X_PAGE_PROGRAM);
    softspi_transfer(spi, (addr >> 16) & 0xFF);
    softspi_transfer(spi, (addr >> 8) & 0xFF);
    softspi_transfer(spi, addr & 0xFF);

    for (size_t i = 0; i < len; i++) {
        softspi_transfer(spi, data[i]);
    }
    softspi_end(spi);

    w25q_wait_busy(spi);
    return true;
}

// 读取数据
bool w25q_read_data(SoftSPI *spi, uint32_t addr, uint8_t *data, size_t len) {
    softspi_begin(spi);
    softspi_transfer(spi, W25X_READ_DATA);
    softspi_transfer(spi, (addr >> 16) & 0xFF);
    softspi_transfer(spi, (addr >> 8) & 0xFF);
    softspi_transfer(spi, addr & 0xFF);

    for (size_t i = 0; i < len; i++) {
        data[i] = softspi_transfer(spi, 0xFF);
    }
    softspi_end(spi);

    return true;
}

// 大容量写入 (自动处理扇区擦除和页编程)
bool w25q_write(SoftSPI *spi, uint32_t addr, const uint8_t *data, size_t len) {
    // 计算涉及的扇区范围
    uint32_t start_sector = addr / W25Q_SECTOR_SIZE;
    uint32_t end_sector = (addr + len - 1) / W25Q_SECTOR_SIZE;

    // 需要逐个扇区处理
    for (uint32_t sector = start_sector; sector <= end_sector; sector++) {
        uint32_t sector_addr = sector * W25Q_SECTOR_SIZE;
        uint32_t sector_offset = (sector == start_sector) ?
                                 (addr % W25Q_SECTOR_SIZE) : 0;
        uint32_t sector_bytes = (sector == end_sector) ?
                                ((addr + len) - sector_addr - sector_offset) :
                                (W25Q_SECTOR_SIZE - sector_offset);

        // 读取整个扇区到缓冲区
        uint8_t sector_buffer[W25Q_SECTOR_SIZE];
        w25q_read_data(spi, sector_addr, sector_buffer, W25Q_SECTOR_SIZE);

        // 修改缓冲区
        memcpy(sector_buffer + sector_offset,
               data + (sector_addr + sector_offset - addr),
               sector_bytes);

        // 擦除扇区
        w25q_sector_erase(spi, sector_addr);

        // 重新编程
        for (int page = 0; page < W25Q_SECTOR_SIZE / W25Q_PAGE_SIZE; page++) {
            uint32_t page_addr = sector_addr + page * W25Q_PAGE_SIZE;
            w25q_page_program(spi, page_addr,
                             sector_buffer + page * W25Q_PAGE_SIZE,
                             W25Q_PAGE_SIZE);
        }
    }

    return true;
}
