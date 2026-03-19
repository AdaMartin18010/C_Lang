/*
 * Auto-generated from: 03_System_Technology_Domains\02_Regex_Engine\01_Thompson_NFA.md
 * Line: 242
 * Language: c
 * Block ID: f5941fe3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 状态列表（当前NFA状态集合）
typedef struct {
    State **s;
    int n;
} List;

static int listid = 0;  // 全局列表ID

// 添加状态到列表（处理ε-闭包）
void addstate(List *l, State *s) {
    if (s == NULL || s->lastlist == listid) {
        return;
    }
    s->lastlist = listid;

    if (s->c == Split) {
        // ε-转移：递归添加两个输出
        addstate(l, s->out);
        addstate(l, s->out1);
    } else {
        // 非ε状态：加入列表
        l->s[l->n++] = s;
    }
}

// 计算状态列表的ε-闭包
List *startlist(State *start, List *l) {
    listid++;
    l->n = 0;
    addstate(l, start);
    return l;
}

// 步骤：根据输入字符转移
List *step(List *clist, int c, List *nlist) {
    int i;
    State *s;

    listid++;
    nlist->n = 0;

    for (i = 0; i < clist->n; i++) {
        s = clist->s[i];
        if (s->c == c || (s->c == 256 && c != '\0')) {  // 256 = 通配符
            addstate(nlist, s->out);
        }
    }

    return nlist;
}

// 匹配函数
int match(State *start, const char *s) {
    int c;
    List *clist, *nlist, *t;

    // 预分配状态列表
    static List l1, l2;
    static State *s1[100], *s2[100];  // 简化：固定大小
    l1.s = s1;
    l2.s = s2;

    clist = startlist(start, &l1);
    nlist = &l2;

    while ((c = *s++) != '\0') {
        clist = step(clist, c, nlist);

        // 交换列表
        t = clist;
        clist = nlist;
        nlist = t;

        if (clist->n == 0) {
            return 0;  // 无匹配状态
        }
    }

    // 检查是否到达匹配状态
    for (int i = 0; i < clist->n; i++) {
        if (clist->s[i]->c == Match) {
            return 1;
        }
    }

    return 0;
}
