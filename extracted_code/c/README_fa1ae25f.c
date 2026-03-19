/*
 * Auto-generated from: 04_Industrial_Scenarios\03_High_Frequency_Trading\README.md
 * Line: 971
 * Language: c
 * Block ID: fa1ae25f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * SBE快速解析 - 零拷贝直接内存访问
 * SBE 1.0标准
 */

/* SBE消息头 */
struct __attribute__((packed)) sbe_header {
    uint16_t block_length;      /* 根块长度 */
    uint16_t template_id;       /* 消息模板ID */
    uint16_t schema_id;         /* Schema ID */
    uint16_t version;           /* Schema版本 */
};

/* 订单簿更新消息 - SBE编码 */
struct __attribute__((packed)) sbe_order_book_update {
    /* 消息头 */
    struct sbe_header header;

    /* 固定字段 */
    uint64_t transact_time;     /* 交易时间 */
    int64_t  price;             /* 价格 */
    int32_t  quantity;          /* 数量 */
    uint32_t order_id;          /* 订单ID */
    uint8_t  side;              /* 买卖方向 */
    uint8_t  update_action;     /* 更新动作 */
    uint16_t padding;

    /* 重复组头部 */
    uint16_t md_entries_count;  /* 条目数量 */
    uint16_t md_entries_length; /* 每个条目长度 */
};

/* 零拷贝解析 */
static inline bool parse_sbe_message(const void *data, size_t len,
                                     struct market_event *event)
{
    const struct sbe_header *hdr = data;

    /* 边界检查 */
    if (len < sizeof(*hdr))
        return false;

    switch (hdr->template_id) {
    case TEMPLATE_ORDER_BOOK_UPDATE: {
        const struct sbe_order_book_update *msg = data;
        if (len < sizeof(*msg))
            return false;

        /* 直接内存映射 - 无需复制 */
        event->timestamp = be64toh(msg->transact_time);
        event->price = be64toh(msg->price);
        event->quantity = be32toh(msg->quantity);
        event->side = msg->side;
        event->action = msg->update_action;

        /* 处理重复组 */
        uint16_t count = be16toh(msg->md_entries_count);
        const char *entries = (const char *)data + sizeof(*msg);

        for (int i = 0; i < count; i++) {
            /* 每个条目在固定偏移处 */
            const struct md_entry *entry =
                (const struct md_entry *)(entries +
                    i * be16toh(msg->md_entries_length));
            /* 处理条目... */
        }
        break;
    }

    case TEMPLATE_TRADE_REPORT:
        /* 处理成交报告 */
        break;

    default:
        return false;  /* 未知消息类型 */
    }

    return true;
}

/* SIMD加速批量解析 */
void parse_sbe_batch_simd(const struct rte_mbuf **pkts, uint16_t nb_pkts,
                          struct market_event *events)
{
    for (uint16_t i = 0; i < nb_pkts; i++) {
        const void *data = rte_pktmbuf_mtod(pkts[i], void *);

        /* 预取下一个 */
        if (i + 4 < nb_pkts) {
            rte_prefetch0(rte_pktmbuf_mtod(pkts[i + 4], void *));
        }

        /* 快速解析 */
        parse_sbe_message(data, pkts[i]->data_len, &events[i]);
    }
}
