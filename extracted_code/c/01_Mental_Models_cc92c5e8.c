/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\01_Mental_Models.md
 * Line: 130
 * Language: c
 * Block ID: cc92c5e8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 概念断裂：混合不同抽象层次
void process_order_bad(Order *order) {
    // 业务逻辑
    validate_order(order);

    // 突然切换到实现细节
    char *sql = malloc(1024);
    sprintf(sql, "UPDATE orders SET status='processed' WHERE id=%d", order->id);
    db_execute(sql);
    free(sql);

    // 又回到业务逻辑
    notify_customer(order);
}

// ✅ 概念对齐：层次分离
void process_order_good(Order *order) {
    validate_order(order);
    update_order_status(order, STATUS_PROCESSED);
    notify_customer(order);
}

void update_order_status(Order *order, Status status) {
    // 数据访问层封装
    repository_update_order_status(order->id, status);
}
