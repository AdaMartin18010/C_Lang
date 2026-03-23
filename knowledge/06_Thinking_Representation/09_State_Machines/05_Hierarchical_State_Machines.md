# 层次状态机(Hierarchical State Machines)

> **层级定位**: 06_Thinking_Representation > 09_State_Machines
> **难度级别**: L5 (高级)
> **前置知识**: [函数指针实现](04_Function_Pointer_Implementation.md), UML状态图
> **预估学习时间**: 3-4 小时

---

## 模块概述

层次状态机(Hierarchical State Machine, HSM)通过状态嵌套解决**状态爆炸**问题。
子状态继承父状态的特性，事件可以先由子状态处理，未处理的则**冒泡**到父状态。这是工业级状态机设计的核心模式。

---

## 学习目标

1. **理解状态嵌套概念** - 父状态、子状态、叶状态的层次关系
2. **实现事件冒泡** - 从子到父的事件传递机制
3. **处理入口/出口链** - 层次状态的进入和退出序列
4. **应用历史状态** - 记住上次离开的子状态

---

## 一、HSM核心概念

### 1.1 层次结构可视化

```
┌─────────────────────────────────────────────────────────────┐
│                    层次状态机结构示例                         │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                                                     │   │
│  │  ┌───────────────┐     ┌───────────────────────┐   │   │
│  │  │               │     │        PLAYING        │   │   │
│  │  │               │     │      (父状态)         │   │   │
│  │  │   STOPPED     │     │  ┌───────┐ ┌───────┐  │   │   │
│  │  │               │────▶│  │ PAUSED│ │NORMAL │  │   │   │
│  │  │               │     │  │(子)   │ │(子)   │  │   │   │
│  │  └───────────────┘     │  └───┬───┘ └───┬───┘  │   │   │
│  │                        │      │         │      │   │   │
│  │                        │  ┌───┴───┐ ┌───┴───┐  │   │   │
│  │                        │  │FFWD   │ │REW    │  │   │   │
│  │                        │  │(叶子) │ │(叶子) │  │   │   │
│  │                        │  └───────┘ └───────┘  │   │   │
│  │                        └───────────────────────┘   │   │
│  │                                                     │   │
│  │  状态总数: 7    扁平实现需要: 7×N事件个转换          │   │
│  │                 层次实现需要: 大大减少重复逻辑       │   │
│  │                                                     │   │
│  └─────────────────────────────────────────────────────┘   │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 核心术语

| 术语 | 定义 | 示例 |
|:-----|:-----|:-----|
| **父状态(Superstate)** | 包含子状态的状态 | PLAYING |
| **子状态(Substate)** | 被父状态包含的状态 | NORMAL, PAUSED |
| **叶状态(Leaf)** | 没有子状态的状态 | FFWD, REW |
| **直接子状态** | 直属父状态下的子状态 | NORMAL是PLAYING的直接子状态 |
| **事件冒泡** | 子状态未处理事件向上传递 | 子→父→祖父... |
| **进入链** | 从根到目标状态的进入序列 | 根→PLAYING→NORMAL |
| **退出链** | 从当前状态到目标的退出序列 | NORMAL→PLAYING→... |

---

## 二、HSM实现框架

### 2.1 数据结构设计

```c
/**
 * @file hsm_framework.c
 * @brief 层次状态机完整实现框架
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* ========== 核心类型 ========== */

typedef struct HsmState HsmState;
typedef struct HsmEvent HsmEvent;
typedef struct Hsm Hsm;

/* 信号类型 */
typedef enum {
    HSM_SIG_EMPTY = 0,   /* 无信号(初始化用) */
    HSM_SIG_ENTRY,       /* 进入状态 */
    HSM_SIG_EXIT,        /* 退出状态 */
    HSM_SIG_INIT,        /* 初始转换(进入默认子状态) */
    HSM_SIG_USER = 4     /* 用户自定义信号从此开始 */
} HsmSignal;

/* 事件结构 */
typedef struct HsmEvent {
    uint32_t sig;        /* 信号类型 */
    void* data;          /* 附加数据 */
} HsmEvent;

/* 状态处理结果 */
typedef uint8_t HsmResult;
#define HSM_RET_HANDLED     ((HsmResult)0)
#define HSM_RET_IGNORED     ((HsmResult)1)
#define HSM_RET_TRAN        ((HsmResult)2)
#define HSM_RET_SUPER       ((HsmResult)3)  /* 需要调用父状态 */

/* 状态函数类型 */
typedef HsmResult (*HsmStateHandler)(Hsm* me, const HsmEvent* e);

/* 状态结构 */
struct HsmState {
    HsmStateHandler handler;     /* 状态处理函数 */
    HsmState* parent;            /* 父状态( NULL = 顶层 ) */
    HsmState* child;             /* 默认子状态(初始转换目标) */
    const char* name;            /* 状态名称(调试) */
};

/* HSM实例 */
struct Hsm {
    HsmState* state;             /* 当前状态 */
    HsmState* source;            /* 转换源状态(内部使用) */
    HsmState* temp;              /* 临时状态(内部使用) */
    void* user_data;             /* 用户数据 */
    uint32_t tran_count;         /* 转换计数 */
};

/* 静态事件定义 */
static const HsmEvent hsm_entry_evt = {HSM_SIG_ENTRY, NULL};
static const HsmEvent hsm_exit_evt  = {HSM_SIG_EXIT, NULL};
static const HsmEvent hsm_init_evt  = {HSM_SIG_INIT, NULL};
```

### 2.2 核心引擎实现

```c
/* ========== HSM核心函数 ========== */

/**
 * @brief 初始化HSM
 */
void hsm_init(Hsm* me, HsmState* initial) {
    me->state = NULL;
    me->source = NULL;
    me->temp = NULL;
    me->tran_count = 0;

    /* 执行初始转换 */
    HsmEvent init_evt = {HSM_SIG_INIT, NULL};

    /* 进入初始状态的祖先链 */
    HsmState* path[16];
    int depth = 0;

    /* 收集从根到initial的路径 */
    for (HsmState* s = initial; s != NULL; s = s->parent) {
        path[depth++] = s;
    }

    /* 从根到initial执行进入 */
    for (int i = depth - 1; i >= 0; i--) {
        me->state = path[i];
        if (path[i]->handler) {
            path[i]->handler(me, &hsm_entry_evt);
        }
    }

    /* 执行初始子状态转换 */
    if (me->state && me->state->child) {
        hsm_tran(me, me->state->child);
    }
}

/**
 * @brief 获取状态的层次深度
 */
static int hsm_depth(HsmState* state) {
    int depth = 0;
    while (state) {
        depth++;
        state = state->parent;
    }
    return depth;
}

/**
 * @brief 找到两个状态的最近公共祖先(LCA)
 */
static HsmState* hsm_lca(HsmState* s1, HsmState* s2) {
    int d1 = hsm_depth(s1);
    int d2 = hsm_depth(s2);

    /* 调整到相同深度 */
    while (d1 > d2) {
        s1 = s1->parent;
        d1--;
    }
    while (d2 > d1) {
        s2 = s2->parent;
        d2--;
    }

    /* 同时向上直到找到公共祖先 */
    while (s1 != s2) {
        s1 = s1->parent;
        s2 = s2->parent;
    }

    return s1;
}

/**
 * @brief 执行状态转换
 */
void hsm_tran(Hsm* me, HsmState* target) {
    if (!me->state || !target) return;

    HsmState* source = me->state;
    HsmState* lca = hsm_lca(source, target);

    /* 1. 执行源状态的退出链(直到LCA,不包括LCA) */
    HsmState* s = source;
    while (s != lca) {
        if (s->handler) {
            s->handler(me, &hsm_exit_evt);
        }
        s = s->parent;
    }

    /* 2. 保存目标路径 */
    HsmState* path[16];
    int depth = 0;
    for (s = target; s != lca; s = s->parent) {
        path[depth++] = s;
    }

    /* 3. 执行目标状态的进入链(从LCA的子状态开始) */
    for (int i = depth - 1; i >= 0; i--) {
        me->state = path[i];
        if (path[i]->handler) {
            path[i]->handler(me, &hsm_entry_evt);
        }
    }

    me->tran_count++;

    /* 4. 执行初始转换(如果有默认子状态) */
    if (me->state->child) {
        HsmResult r = me->state->handler(me, &hsm_init_evt);
        if (r == HSM_RET_TRAN && me->temp) {
            hsm_tran(me, me->temp);
        }
    }
}

/**
 * @brief 分发事件(支持冒泡)
 */
void hsm_dispatch(Hsm* me, const HsmEvent* e) {
    HsmState* s = me->state;

    /* 尝试当前状态及其祖先 */
    while (s) {
        HsmResult r = s->handler(me, e);

        if (r != HSM_RET_SUPER) {
            /* 事件被处理(或被忽略) */
            return;
        }

        /* 冒泡到父状态 */
        s = s->parent;
    }

    /* 没有任何状态处理此事件 */
    printf("[HSM] 警告: 事件 %d 未被任何状态处理\n", e->sig);
}
```

---

## 三、完整案例: 媒体播放器HSM

```c
/**
 * @file media_player_hsm.c
 * @brief 媒体播放器的层次状态机实现
 */

#include <stdio.h>
#include "hsm_framework.c"  /* 包含上面定义的框架 */

/* ========== 信号定义 ========== */

typedef enum {
    /* 标准信号 */
    SIG_PLAY = HSM_SIG_USER,
    SIG_PAUSE,
    SIG_STOP,
    SIG_NEXT,
    SIG_PREV,
    SIG_FFWD,       /* 快进 */
    SIG_FREW,       /* 快退 */
    SIG_FFWD_RELEASE,
    SIG_FREW_RELEASE,
    SIG_EOS,        /* 播放结束 */
} PlayerSignals;

/* ========== 状态声明 ========== */

HsmResult state_top(Hsm* me, const HsmEvent* e);      /* 顶层 */
HsmResult state_stopped(Hsm* me, const HsmEvent* e);  /* 停止 */
HsmResult state_playing(Hsm* me, const HsmEvent* e);  /* 播放(父) */
HsmResult state_normal(Hsm* me, const HsmEvent* e);   /* 正常播放 */
HsmResult state_paused(Hsm* me, const HsmEvent* e);   /* 暂停 */
HsmResult state_ffwd(Hsm* me, const HsmEvent* e);     /* 快进 */
HsmResult state_frew(Hsm* me, const HsmEvent* e);     /* 快退 */

/* ========== 状态对象 ========== */

static HsmState state_top_obj = {
    .handler = state_top,
    .parent = NULL,
    .child = NULL,
    .name = "TOP"
};

static HsmState state_stopped_obj = {
    .handler = state_stopped,
    .parent = &state_top_obj,
    .child = NULL,
    .name = "STOPPED"
};

static HsmState state_playing_obj = {
    .handler = state_playing,
    .parent = &state_top_obj,
    .child = &state_normal_obj,  /* 默认子状态 */
    .name = "PLAYING"
};

static HsmState state_normal_obj = {
    .handler = state_normal,
    .parent = &state_playing_obj,
    .child = NULL,
    .name = "NORMAL"
};

static HsmState state_paused_obj = {
    .handler = state_paused,
    .parent = &state_playing_obj,
    .child = NULL,
    .name = "PAUSED"
};

static HsmState state_ffwd_obj = {
    .handler = state_ffwd,
    .parent = &state_playing_obj,
    .child = NULL,
    .name = "FFWD"
};

static HsmState state_frew_obj = {
    .handler = state_frew,
    .parent = &state_playing_obj,
    .child = NULL,
    .name = "FREW"
};

/* ========== 状态函数实现 ========== */

/* 顶层状态: 处理所有状态共享的事件 */
HsmResult state_top(Hsm* me, const HsmEvent* e) {
    switch (e->sig) {
        case HSM_SIG_ENTRY:
            printf("  [TOP] 进入\n");
            return HSM_RET_HANDLED;

        case HSM_SIG_EXIT:
            printf("  [TOP] 退出\n");
            return HSM_RET_HANDLED;

        case HSM_SIG_INIT:
            /* 初始转换到STOPPED */
            me->temp = &state_stopped_obj;
            return HSM_RET_TRAN;

        default:
            return HSM_RET_IGNORED;
    }
}

/* STOPPED状态 */
HsmResult state_stopped(Hsm* me, const HsmEvent* e) {
    switch (e->sig) {
        case HSM_SIG_ENTRY:
            printf("  [STOPPED] 进入: 停止播放，复位进度\n");
            return HSM_RET_HANDLED;

        case HSM_SIG_EXIT:
            printf("  [STOPPED] 退出\n");
            return HSM_RET_HANDLED;

        case SIG_PLAY:
            printf("  [STOPPED] PLAY → PLAYING\n");
            me->temp = &state_playing_obj;
            return HSM_RET_TRAN;

        default:
            /* 其他事件不处理，冒泡到TOP */
            return HSM_RET_SUPER;
    }
}

/* PLAYING父状态: 处理PLAYING下所有子状态共享的事件 */
HsmResult state_playing(Hsm* me, const HsmEvent* e) {
    switch (e->sig) {
        case HSM_SIG_ENTRY:
            printf("  [PLAYING] 进入: 初始化播放\n");
            return HSM_RET_HANDLED;

        case HSM_SIG_EXIT:
            printf("  [PLAYING] 退出: 清理播放资源\n");
            return HSM_RET_HANDLED;

        case HSM_SIG_INIT:
            /* 初始转换到NORMAL子状态 */
            me->temp = &state_normal_obj;
            return HSM_RET_TRAN;

        case SIG_STOP:
            printf("  [PLAYING] STOP → STOPPED (父状态处理)\n");
            me->temp = &state_stopped_obj;
            return HSM_RET_TRAN;

        case SIG_PAUSE:
            printf("  [PLAYING] PAUSE → PAUSED (父状态处理)\n");
            me->temp = &state_paused_obj;
            return HSM_RET_TRAN;

        default:
            /* 其他事件让子状态处理 */
            return HSM_RET_SUPER;
    }
}

/* NORMAL播放状态 */
HsmResult state_normal(Hsm* me, const HsmEvent* e) {
    switch (e->sig) {
        case HSM_SIG_ENTRY:
            printf("  [NORMAL] 进入: 正常速度播放\n");
            return HSM_RET_HANDLED;

        case HSM_SIG_EXIT:
            printf("  [NORMAL] 退出\n");
            return HSM_RET_HANDLED;

        case SIG_FFWD:
            printf("  [NORMAL] FFWD按下 → FFWD\n");
            me->temp = &state_ffwd_obj;
            return HSM_RET_TRAN;

        case SIG_FREW:
            printf("  [NORMAL] FREW按下 → FREW\n");
            me->temp = &state_frew_obj;
            return HSM_RET_TRAN;

        case SIG_NEXT:
            printf("  [NORMAL] 下一曲\n");
            return HSM_RET_HANDLED;

        case SIG_PREV:
            printf("  [NORMAL] 上一曲\n");
            return HSM_RET_HANDLED;

        case SIG_EOS:
            printf("  [NORMAL] 播放结束 → STOPPED\n");
            me->temp = &state_stopped_obj;
            return HSM_RET_TRAN;

        default:
            /* PAUSE/STOP让父状态PLAYING处理 */
            return HSM_RET_SUPER;
    }
}

/* PAUSED状态 */
HsmResult state_paused(Hsm* me, const HsmEvent* e) {
    switch (e->sig) {
        case HSM_SIG_ENTRY:
            printf("  [PAUSED] 进入: 暂停播放，保存位置\n");
            return HSM_RET_HANDLED;

        case HSM_SIG_EXIT:
            printf("  [PAUSED] 退出\n");
            return HSM_RET_HANDLED;

        case SIG_PAUSE:
        case SIG_PLAY:
            printf("  [PAUSED] PLAY/PAUSE → NORMAL\n");
            me->temp = &state_normal_obj;
            return HSM_RET_TRAN;

        default:
            return HSM_RET_SUPER;
    }
}

/* 快进状态 */
HsmResult state_ffwd(Hsm* me, const HsmEvent* e) {
    switch (e->sig) {
        case HSM_SIG_ENTRY:
            printf("  [FFWD] 进入: 2倍速前进\n");
            return HSM_RET_HANDLED;

        case HSM_SIG_EXIT:
            printf("  [FFWD] 退出: 恢复正常速度\n");
            return HSM_RET_HANDLED;

        case SIG_FFWD_RELEASE:
            printf("  [FFWD] 释放 → NORMAL\n");
            me->temp = &state_normal_obj;
            return HSM_RET_TRAN;

        default:
            return HSM_RET_SUPER;
    }
}

/* 快退状态 */
HsmResult state_frew(Hsm* me, const HsmEvent* e) {
    switch (e->sig) {
        case HSM_SIG_ENTRY:
            printf("  [FREW] 进入: 2倍速后退\n");
            return HSM_RET_HANDLED;

        case HSM_SIG_EXIT:
            printf("  [FREW] 退出: 恢复正常速度\n");
            return HSM_RET_HANDLED;

        case SIG_FREW_RELEASE:
            printf("  [FREW] 释放 → NORMAL\n");
            me->temp = &state_normal_obj;
            return HSM_RET_TRAN;

        default:
            return HSM_RET_SUPER;
    }
}

/* ========== 测试 ========== */

int main(void) {
    Hsm player;
    player.user_data = NULL;

    printf("=== 媒体播放器HSM测试 ===\n\n");

    /* 初始化HSM */
    printf("【初始化】\n");
    hsm_init(&player, &state_top_obj);
    printf("当前状态: %s\n\n", player.state->name);

    /* 测试场景 */
    HsmEvent evt;

    printf("【场景1: 播放】\n");
    evt.sig = SIG_PLAY;
    hsm_dispatch(&player, &evt);
    printf("当前状态: %s\n\n", player.state->name);

    printf("【场景2: 暂停】\n");
    evt.sig = SIG_PAUSE;
    hsm_dispatch(&player, &evt);
    printf("当前状态: %s\n\n", player.state->name);

    printf("【场景3: 恢复播放】\n");
    evt.sig = SIG_PLAY;
    hsm_dispatch(&player, &evt);
    printf("当前状态: %s\n\n", player.state->name);

    printf("【场景4: 快进】\n");
    evt.sig = SIG_FFWD;
    hsm_dispatch(&player, &evt);
    printf("当前状态: %s\n\n", player.state->name);

    printf("【场景5: 停止快进】\n");
    evt.sig = SIG_FFWD_RELEASE;
    hsm_dispatch(&player, &evt);
    printf("当前状态: %s\n\n", player.state->name);

    printf("【场景6: 停止播放】\n");
    evt.sig = SIG_STOP;
    hsm_dispatch(&player, &evt);
    printf("当前状态: %s\n\n", player.state->name);

    printf("总转换次数: %u\n", player.tran_count);

    return 0;
}
```

---

## 四、思维表征: HSM设计决策树

```
┌─────────────────────────────────────────────────────────────┐
│                层次状态机设计决策流程                         │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  状态数 > 20? ──否──▶ 考虑Switch-Case或查表法               │
│      │                                                       │
│     是                                                       │
│      ▼                                                       │
│  存在状态分组? ──否──▶ 扁平状态机+良好命名                  │
│      │                                                       │
│     是                                                       │
│      ▼                                                       │
│  组内状态共享行为? ──否──▶ 使用查表法                       │
│      │                                                       │
│     是                                                       │
│      ▼                                                       │
│  ┌─────────────────────────────────────────┐                │
│  │  使用层次状态机(HSM)                    │                │
│  │                                         │                │
│  │  设计步骤:                              │                │
│  │  1. 识别父状态(共享行为的组)            │                │
│  │  2. 识别叶状态(具体行为)                │                │
│  │  3. 设计事件冒泡路径                    │                │
│  │  4. 实现进入/退出链                     │                │
│  │  5. 添加历史状态(如需要)                │                │
│  └─────────────────────────────────────────┘                │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 五、高级主题

### 5.1 历史状态 (History State)

```c
/**
 * @brief 浅历史状态实现 - 记住父状态的直接子状态
 */

typedef struct {
    Hsm base;
    HsmState* playing_history;  /* 记住PLAYING的最后子状态 */
} PlayerHsmWithHistory;

HsmResult state_playing_with_history(Hsm* me, const HsmEvent* e) {
    PlayerHsmWithHistory* player = (PlayerHsmWithHistory*)me;

    switch (e->sig) {
        case HSM_SIG_EXIT:
            /* 保存历史 */
            player->playing_history = me->state;
            return HSM_RET_HANDLED;

        case HSM_SIG_INIT:
            /* 恢复历史，如果没有则使用默认 */
            if (player->playing_history) {
                me->temp = player->playing_history;
            } else {
                me->temp = &state_normal_obj;
            }
            return HSM_RET_TRAN;

        default:
            return state_playing(me, e);
    }
}
```

### 5.2 正交区域 (Orthogonal Regions)

```c
/**
 * @brief 正交区域 - 同时处于多个独立状态
 * @description 例如: 播放状态 + 网络状态 可以同时变化
 */

typedef struct {
    Hsm audio_fsm;   /* 音频播放状态机 */
    Hsm network_fsm; /* 网络连接状态机 */
} OrthogonalPlayer;

void orthogonal_dispatch(OrthogonalPlayer* player, const HsmEvent* e) {
    /* 同时分发到两个正交区域 */
    hsm_dispatch(&player->audio_fsm, e);
    hsm_dispatch(&player->network_fsm, e);
}
```

---

## 六、知识检验

### 自测题

1. **什么是LCA(最近公共祖先)? 为什么在状态转换中重要?**

2. **父状态处理的事件，子状态还会收到吗?**

3. **如何实现"记住上次播放位置"这样的功能?**

4. **HSM相比扁平状态机，代码量通常减少多少?**

### 代码练习

**练习: 电梯控制系统HSM**

设计一个3层电梯的状态机：

- 父状态: IDLE / MOVING / DOOR_OPEN
- MOVING子状态: GOING_UP / GOING_DOWN
- 要求实现：楼层请求队列、门开超时、紧急停止

---

## 七、延伸阅读

- [QP Framework](https://www.state-machine.com/) - 工业级HSM框架
- ["Practical UML Statecharts in C/C++"](https://www.state-machine.com/psicc2) - HSM权威著作

---

**最后更新**: 2026-03-24
**维护者**: C_Lang Knowledge Base Team
**质量等级**: L5 (专家级)


## 🧠 实质性思维方法

### 认知科学基础

#### 1. 认知负荷理论

- **内在负荷**: 概念本身的复杂度
- **外在负荷**: 信息呈现方式
- **相关负荷**: 构建图式的认知投入

#### 2. 知识结构化

```
表层结构 → 深层结构 → 抽象原理
   │            │           │
   └────────────┴───────────┘
              │
              ▼
           迁移应用
```

#### 3. 元认知策略

| 策略 | 应用 | 效果 |
|:-----|:-----|:-----|
| 自我解释 | 解释代码行为 | 深度理解 |
| 预测验证 | 预测输出 | 调试能力 |
| 类比推理 | 联系已知 | 知识迁移 |

### 实践关联

- [概念映射](05_Concept_Mappings/README.md)
- [学习路径](06_Learning_Paths/README.md)
- [决策树](01_Decision_Trees/README.md)
