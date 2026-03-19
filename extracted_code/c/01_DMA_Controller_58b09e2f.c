/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\10_IO_System\01_DMA_Controller.md
 * Line: 742
 * Language: c
 * Block ID: 58b09e2f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux DMA Cache一致性API
 * include/linux/dma-mapping.h
 */

// 方向定义
define DMA_BIDIRECTIONAL   0
#define DMA_TO_DEVICE       1
#define DMA_FROM_DEVICE     2
#define DMA_NONE            3

// 一致性DMA缓冲区分配 (Coherent/Consistent)
// 保证CPU和DMA设备看到一致的内存视图
void *dma_alloc_coherent(struct device *dev, size_t size,
                          dma_addr_t *dma_handle, gfp_t flag);

void dma_free_coherent(struct device *dev, size_t size,
                        void *cpu_addr, dma_addr_t dma_handle);

// 使用示例
dma_addr_t dma_handle;
void *cpu_addr;

// 分配一致性缓冲区
cpu_addr = dma_alloc_coherent(dev, 4096, &dma_handle, GFP_KERNEL);
if (!cpu_addr)
    return -ENOMEM;

// CPU写入数据
cpu_addr[0] = 0xAA;
cpu_addr[1] = 0xBB;

// DMA可以直接读取，无需同步
// 设备使用 dma_handle 作为物理地址

// 完成后释放
dma_free_coherent(dev, 4096, cpu_addr, dma_handle);


// 流式DMA映射 (Streaming) - 需要显式同步
// 适用于动态数据，性能更高

dma_addr_t dma_handle;
void *buffer = kmalloc(4096, GFP_KERNEL);

// 1. 映射缓冲区给DMA使用
dma_handle = dma_map_single(dev, buffer, 4096, DMA_TO_DEVICE);
if (dma_mapping_error(dev, dma_handle))
    return -ENOMEM;

// 2. DMA TO DEVICE: CPU写入完成后同步
cpu_fill_buffer(buffer);
dma_sync_single_for_device(dev, dma_handle, 4096, DMA_TO_DEVICE);
// 现在设备可以安全读取

// 3. 设备读取完成后取消映射
dma_unmap_single(dev, dma_handle, 4096, DMA_TO_DEVICE);


// DMA FROM DEVICE 场景
void *buffer = kmalloc(4096, GFP_KERNEL);
dma_addr_t dma_handle = dma_map_single(dev, buffer, 4096, DMA_FROM_DEVICE);

// 设备写入数据...

// 4. 设备写入完成后同步
dma_sync_single_for_cpu(dev, dma_handle, 4096, DMA_FROM_DEVICE);
// 现在CPU可以安全读取
read_buffer(buffer);

dma_unmap_single(dev, dma_handle, 4096, DMA_FROM_DEVICE);


// 分散/聚集映射
dma_addr_t dma_handle;
int nents;
struct scatterlist sg[3];

// 填充SG列表...
nents = dma_map_sg(dev, sg, 3, DMA_BIDIRECTIONAL);

// 同步
// 设备读取前:
dma_sync_sg_for_device(dev, sg, nents, DMA_TO_DEVICE);

// 设备写入后:
dma_sync_sg_for_cpu(dev, sg, nents, DMA_FROM_DEVICE);

dma_unmap_sg(dev, sg, 3, DMA_BIDIRECTIONAL);
