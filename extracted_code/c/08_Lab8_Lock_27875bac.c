/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\08_Lab8_Lock.md
 * Line: 179
 * Language: c
 * Block ID: 27875bac
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/spinlock.c

// 保存中断状态并关中断
void
push_off(void)
{
    int old = intr_get();

    // 保存旧状态
    intr_off();

    // 如果是第一次关中断，保存旧状态
    if (mycpu()->noff == 0)
        mycpu()->intena = old;

    mycpu()->noff += 1;
}

// 恢复中断状态
void
pop_off(void)
{
    struct cpu *c = mycpu();

    // 检查：是否在关中断状态
    if (intr_get())
        panic("pop_off - interruptible");

    // 检查：嵌套计数是否正确
    if (c->noff < 1)
        panic("pop_off");

    c->noff -= 1;

    // 只有当所有嵌套都退出后才开中断
    if (c->noff == 0 && c->intena)
        intr_on();
}
