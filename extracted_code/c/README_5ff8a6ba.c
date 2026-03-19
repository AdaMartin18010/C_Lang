/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\README.md
 * Line: 841
 * Language: c
 * Block ID: 5ff8a6ba
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 重构后：可测试的架构

// 1. 定义数据访问接口
typedef struct account_repository account_repository_t;

struct account_repository {
    // 虚函数表
    account_t* (*find_by_id)(account_repository_t* self, int id);
    int (*update)(account_repository_t* self, account_t* account);
    int (*begin_transaction)(account_repository_t* self);
    int (*commit)(account_repository_t* self);
    int (*rollback)(account_repository_t* self);
    void (*destroy)(account_repository_t* self);
    void* impl_data;  // 实现特定数据
};

// 2. MySQL 实现
account_repository_t* account_repository_mysql_create(const db_config_t* config);

// 3. 内存实现（用于测试）
typedef struct {
    account_t* accounts;
    size_t count;
    size_t capacity;
    bool in_transaction;
    account_t* backup;  // 事务回滚用
} memory_repo_impl_t;

account_repository_t* account_repository_memory_create(void) {
    account_repository_t* repo = calloc(1, sizeof(account_repository_t));
    memory_repo_impl_t* impl = calloc(1, sizeof(memory_repo_impl_t));

    impl->capacity = 16;
    impl->accounts = calloc(impl->capacity, sizeof(account_t));

    repo->impl_data = impl;
    repo->find_by_id = memory_find_by_id;
    repo->update = memory_update;
    repo->begin_transaction = memory_begin_transaction;
    repo->commit = memory_commit;
    repo->rollback = memory_rollback;
    repo->destroy = memory_destroy;

    return repo;
}

// 4. 业务逻辑 - 现在可测试
typedef struct {
    account_repository_t* account_repo;
    transaction_repository_t* transaction_repo;
    notification_service_t* notification_service;
} transfer_service_t;

transfer_result_t transfer_funds_improved(transfer_service_t* service,
                                          int from_id, int to_id,
                                          double amount) {
    // 验证输入
    if (amount <= 0) {
        return TRANSFER_INVALID_AMOUNT;
    }
    if (from_id == to_id) {
        return TRANSFER_SAME_ACCOUNT;
    }

    // 查找账户
    account_t* from = service->account_repo->find_by_id(service->account_repo, from_id);
    if (!from) return TRANSFER_ACCOUNT_NOT_FOUND;

    account_t* to = service->account_repo->find_by_id(service->account_repo, to_id);
    if (!to) return TRANSFER_ACCOUNT_NOT_FOUND;

    // 检查余额
    if (from->balance < amount) {
        return TRANSFER_INSUFFICIENT_FUNDS;
    }

    // 执行转账
    service->account_repo->begin_transaction(service->account_repo);

    from->balance -= amount;
    to->balance += amount;

    int result1 = service->account_repo->update(service->account_repo, from);
    int result2 = service->account_repo->update(service->account_repo, to);

    if (result1 != 0 || result2 != 0) {
        service->account_repo->rollback(service->account_repo);
        return TRANSFER_UPDATE_FAILED;
    }

    service->account_repo->commit(service->account_repo);

    // 发送通知
    service->notification_service->notify_transfer(
        service->notification_service, from_id, to_id, amount);

    return TRANSFER_SUCCESS;
}

// 5. 单元测试示例
void test_transfer_success() {
    // 使用内存仓库进行测试
    account_repository_t* repo = account_repository_memory_create();

    // 准备测试数据
    account_t acc1 = { .id = 1, .name = "Alice", .balance = 1000.0 };
    account_t acc2 = { .id = 2, .name = "Bob", .balance = 500.0 };
    repo->update(repo, &acc1);
    repo->update(repo, &acc2);

    // 创建服务
    transfer_service_t service = {
        .account_repo = repo,
        .transaction_repo = NULL,
        .notification_service = mock_notification_service()
    };

    // 执行测试
    transfer_result_t result = transfer_funds_improved(&service, 1, 2, 100.0);

    // 验证结果
    assert(result == TRANSFER_SUCCESS);
    account_t* updated1 = repo->find_by_id(repo, 1);
    account_t* updated2 = repo->find_by_id(repo, 2);
    assert(updated1->balance == 900.0);
    assert(updated2->balance == 600.0);

    repo->destroy(repo);
}
