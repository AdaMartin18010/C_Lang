/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\01_Mental_Models.md
 * Line: 161
 * Language: c
 * Block ID: 2dfcb125
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 领域语言与代码同构

// 银行业务领域
typedef struct {
    AccountNumber account_id;
    Money balance;
    Currency currency;
    TransactionHistory *history;
} BankAccount;

// 使用领域术语
Result transfer_money(BankAccount *from,
                       BankAccount *to,
                       Money amount) {
    if (!has_sufficient_funds(from, amount)) {
        return Result_insufficient_funds;
    }

    DebitTransaction *debit = create_debit(from, amount);
    CreditTransaction *credit = create_credit(to, amount);

    return execute_atomic_transfer(debit, credit);
}
