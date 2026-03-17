# Multi-Raft分布式共识


---

## 📑 目录

- [Multi-Raft分布式共识](#multi-raft分布式共识)
  - [📑 目录](#-目录)
  - [一、Raft算法回顾](#一raft算法回顾)
    - [1.1 Raft核心概念](#11-raft核心概念)
    - [1.2 Raft基本流程](#12-raft基本流程)
    - [1.3 Raft安全性保证](#13-raft安全性保证)
  - [二、Multi-Raft设计](#二multi-raft设计)
    - [2.1 为什么需要Multi-Raft](#21-为什么需要multi-raft)
    - [2.2 Multi-Raft核心架构](#22-multi-raft核心架构)
    - [2.3 消息路由与处理](#23-消息路由与处理)
    - [2.4 Batch优化](#24-batch优化)
  - [三、数据分片](#三数据分片)
    - [3.1 Region设计](#31-region设计)
    - [3.2 分片路由](#32-分片路由)
    - [3.3 Region分裂与合并](#33-region分裂与合并)
  - [四、领导均衡](#四领导均衡)
    - [4.1 领导分布问题](#41-领导分布问题)
    - [4.2 领导均衡调度器](#42-领导均衡调度器)
    - [4.3 领导转移实现](#43-领导转移实现)
  - [五、成员变更](#五成员变更)
    - [5.1 Joint Consensus成员变更](#51-joint-consensus成员变更)
    - [5.2 成员变更流程](#52-成员变更流程)
    - [5.3 Learner节点](#53-learner节点)
  - [六、TiKV实现参考](#六tikv实现参考)
    - [6.1 TiKV架构概览](#61-tikv架构概览)
    - [6.2 TiKV关键代码结构](#62-tikv关键代码结构)
    - [6.3 Local Reader优化](#63-local-reader优化)
    - [6.4 Multi-Raft性能优化总结](#64-multi-raft性能优化总结)
  - [参考资料](#参考资料)


---

## 一、Raft算法回顾

### 1.1 Raft核心概念

Raft是一种为了管理复制日志而设计的共识算法，由Diego Ongaro和John Ousterhout在2014年提出。
相比于Paxos，Raft设计的主要目标是可理解性（Understandability）。

**Raft算法核心状态：**

```go
// 节点状态枚举
type NodeState int

const (
    Follower NodeState = iota
    Candidate
    Leader
)

// Raft节点核心数据结构
type RaftNode struct {
    // 持久化状态
    CurrentTerm int64       // 当前任期号
    VotedFor    int64       // 投票给哪个候选者
    Log         []LogEntry  // 日志条目

    // 易失性状态
    CommitIndex int64       // 已提交的最高日志索引
    LastApplied int64       // 已应用到状态机的最高索引

    // Leader特有状态
    NextIndex   map[int64]int64   // 每个跟随者的下一个日志索引
    MatchIndex  map[int64]int64   // 每个跟随者已复制的最高索引

    // 运行时状态
    State       NodeState   // 当前节点状态
    LeaderID    int64       // 当前Leader ID
}

type LogEntry struct {
    Term    int64       // 创建该条目的任期
    Index   int64       // 条目在日志中的索引
    Command interface{} // 状态机命令
}
```

### 1.2 Raft基本流程

**Leader选举流程：**

```
┌─────────────┐     超时未收到心跳     ┌─────────────┐
│  Follower   │ ─────────────────────▶ │  Candidate  │
└─────────────┘                        └─────────────┘
                                              │
                                              │ 发起选举
                                              ▼
                                       ┌─────────────┐
                                       │  增加任期号  │
                                       │  给自己投票  │
                                       │  发送RV请求  │
                                       └─────────────┘
                                              │
                    ┌─────────────────────────┼─────────────────────────┐
                    ▼                         ▼                         ▼
              获得多数票                    收到更高任期                  超时
           ┌─────────────┐            ┌─────────────┐            ┌─────────────┐
           │    Leader   │            │  Follower   │            │  Candidate  │
           └─────────────┘            └─────────────┘            └─────────────┘
```

**日志复制机制：**

1. **客户端请求**：客户端向Leader发送命令
2. **追加日志**：Leader将命令追加到本地日志
3. **并行复制**：Leader并行向所有Follower发送AppendEntries RPC
4. **提交确认**：当多数节点确认复制成功，Leader提交该条目
5. **应用状态机**：Leader和Follower将已提交的日志应用到状态机
6. **响应客户端**：Leader向客户端返回执行结果

```go
// AppendEntries RPC结构
type AppendEntriesRequest struct {
    Term         int64       // Leader的任期
    LeaderID     int64       // Leader ID（用于重定向客户端）
    PrevLogIndex int64       // 新日志条目之前的索引
    PrevLogTerm  int64       // PrevLogIndex对应条目的任期
    Entries      []LogEntry  // 要存储的日志条目（心跳时为空）
    LeaderCommit int64       // Leader的CommitIndex
}

type AppendEntriesResponse struct {
    Term       int64  // 当前任期（用于Leader更新自己）
    Success    bool   // 是否成功匹配PrevLog
    NextIndex  int64  // 建议的下一个索引（优化版）
    ConflictTerm int64 // 冲突任期（快速回退优化）
}
```

### 1.3 Raft安全性保证

**选举限制（Election Restriction）：**

```go
// RequestVote RPC处理中的安全限制
func (r *RaftNode) canVoteFor(candidateLastTerm, candidateLastIndex int64) bool {
    lastTerm := r.getLastLogTerm()
    lastIndex := r.getLastLogIndex()

    // 候选者的日志必须至少与投票者一样新
    // 比较规则：先比较任期，再比较索引
    return (candidateLastTerm > lastTerm) ||
           (candidateLastTerm == lastTerm && candidateLastIndex >= lastIndex)
}
```

**提交规则（Commitment Rule）：**

```go
// Leader只能提交当前任期的日志条目
func (r *RaftNode) advanceCommitIndex() {
    // 计算MatchIndex的中位数（多数派）
    matchIndexes := make([]int64, 0, len(r.MatchIndex))
    for _, idx := range r.MatchIndex {
        matchIndexes = append(matchIndexes, idx)
    }
    sort.Slice(matchIndexes, func(i, j int) bool {
        return matchIndexes[i] > matchIndexes[j]
    })

    majorityIndex := matchIndexes[len(matchIndexes)/2]

    // 安全提交：只提交当前任期的条目
    if majorityIndex > r.CommitIndex {
        entry := r.Log[majorityIndex]
        if entry.Term == r.CurrentTerm {
            r.CommitIndex = majorityIndex
        }
    }
}
```

---

## 二、Multi-Raft设计

### 2.1 为什么需要Multi-Raft

**单Raft的局限性：**

| 问题 | 描述 | 影响 |
|------|------|------|
| 吞吐量瓶颈 | 单Leader处理所有写请求 | 写入性能受限于单节点 |
| 存储限制 | 所有数据复制到所有节点 | 总容量等于单节点容量 |
| 地理延迟 | 跨地域复制增加延迟 | 远距离节点影响性能 |
| 资源不均衡 | Leader节点负载过重 | CPU/网络资源分配不均 |

**Multi-Raft解决方案：**

将数据划分为多个**Region**（分区），每个Region是一个独立的Raft组，拥有自己独立的Leader和成员。

```
┌─────────────────────────────────────────────────────────────┐
│                        集群节点池                            │
│  ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐  │
│  │ Node A  │    │ Node B  │    │ Node C  │    │ Node D  │  │
│  │         │    │         │    │         │    │         │  │
│  │ Raft-1  │    │ Raft-2  │    │ Raft-3  │    │ Raft-1  │  │
│  │ (Leader)│    │ (Leader)│    │ (Leader)│    │ (Follow)│  │
│  │ Raft-3  │    │ Raft-1  │    │ Raft-2  │    │ Raft-2  │  │
│  │ (Follow)│    │ (Follow)│    │ (Follow)│    │ (Follow)│  │
│  │ Raft-2  │    │ Raft-3  │    │ Raft-1  │    │ Raft-3  │  │
│  │ (Follow)│    │ (Follow)│    │ (Follow)│    │ (Follow)│  │
│  └─────────┘    └─────────┘    └─────────┘    └─────────┘  │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 Multi-Raft核心架构

```go
// Multi-Raft管理器
type MultiRaft struct {
    // 所有Raft组的管理
    raftGroups map[uint64]*RaftGroup  // RegionID -> RaftGroup

    // 节点到Raft组的映射（用于消息路由）
    nodeGroups map[uint64]map[uint64]struct{}  // NodeID -> Set<RegionID>

    // 传输层
    transport Transport

    // 存储引擎
    storage Storage

    // 配置
    config *Config
}

// 单个Raft组
type RaftGroup struct {
    RegionID    uint64           // 区域唯一标识
    RaftState   *raft.RawNode    // 底层Raft状态机
    PeerCache   map[uint64]*Peer // PeerID -> Peer信息
    ApplyState  ApplyState       // 应用状态

    // 读写通道
    proposals   chan *Proposal   // 提案队列
    applyCh     chan ApplyTask   // 应用任务队列
}

type Peer struct {
    ID          uint64
    StoreID     uint64
    IsLearner   bool
    LastContact time.Time
}
```

### 2.3 消息路由与处理

**Raft消息封装：**

```go
// RaftMessage 封装跨Raft组的消息
type RaftMessage struct {
    RegionID    uint64        // 目标Region
    FromPeer    *Peer         // 发送方
    ToPeer      *Peer         // 接收方
    Message     raftpb.Message // 底层Raft消息

    // 用于性能优化
    DiskUsage   uint64        // 发送方磁盘使用情况
    RejectHint  uint64        // 拒绝时的提示信息
}

// 消息处理流程
func (mr *MultiRaft) handleRaftMessage(msg *RaftMessage) error {
    // 1. 获取或创建对应的Raft组
    group, err := mr.getOrCreateRaftGroup(msg.RegionID, msg.ToPeer)
    if err != nil {
        return err
    }

    // 2. 将消息传递给对应的Raft状态机
    if err := group.RaftState.Step(msg.Message); err != nil {
        return err
    }

    // 3. 处理Ready（待发送消息、持久化日志、提交条目等）
    if group.RaftState.HasReady() {
        ready := group.RaftState.Ready()
        mr.handleReady(group, ready)
    }

    return nil
}
```

### 2.4 Batch优化

```go
// 批量处理Ready以提升性能
type ReadyBatch struct {
    // 批量持久化
    entriesToSave []raftpb.Entry

    // 批量发送消息
    messages      []*RaftMessage

    // 批量应用
    committedEntries []raftpb.Entry

    // 批量更新元数据
    snapshots    []raftpb.Snapshot
    hardStates   []raftpb.HardState
}

func (mr *MultiRaft) batchHandleReadys() {
    batch := &ReadyBatch{}

    // 收集所有Raft组的Ready
    for regionID, group := range mr.raftGroups {
        if !group.RaftState.HasReady() {
            continue
        }

        ready := group.RaftState.Ready()

        // 聚合条目
        batch.entriesToSave = append(batch.entriesToSave, ready.Entries...)

        // 聚合消息
        for _, msg := range ready.Messages {
            batch.messages = append(batch.messages, &RaftMessage{
                RegionID: regionID,
                Message:  msg,
            })
        }

        // 聚合已提交条目
        batch.committedEntries = append(batch.committedEntries, ready.CommittedEntries...)
    }

    // 批量写入存储
    mr.storage.Append(batch.entriesToSave)

    // 批量发送消息（按目标节点聚合）
    mr.transport.SendBatch(batch.messages)

    // 批量应用到状态机
    mr.applyCommittedEntries(batch.committedEntries)
}
```

---

## 三、数据分片

### 3.1 Region设计

```go
// Region表示一个数据分片
type Region struct {
    ID          uint64    // Region唯一ID
    StartKey    []byte    // 范围起始键（包含）
    EndKey      []byte    // 范围结束键（不包含）
    RegionEpoch *RegionEpoch // 版本信息，用于处理分裂/合并
    Peers       []*Peer   // 该Region的所有副本
}

// RegionEpoch 用于检测Region配置变化
type RegionEpoch struct {
    ConfVer uint64  // 配置版本（成员变更时递增）
    Version uint64  // 范围版本（分裂/合并时递增）
}

// 示例：Region范围划分
// Region 1: ["", "user_1000")
// Region 2: ["user_1000", "user_2000")
// Region 3: ["user_2000", "")
```

### 3.2 分片路由

```go
// RegionRouter 管理Region路由信息
type RegionRouter struct {
    // 使用跳表或B树存储Region范围信息
    regions *btree.BTree  // key: EndKey, value: *Region

    // 缓存最近访问的Region
    cache   *lru.Cache

    // 读写锁
    mu      sync.RWMutex
}

// 根据Key查找所属Region
func (rr *RegionRouter) GetRegionByKey(key []byte) (*Region, error) {
    rr.mu.RLock()
    defer rr.mu.RUnlock()

    // 先查缓存
    if region, ok := rr.cache.Get(string(key)); ok {
        return region.(*Region), nil
    }

    // 在B树中查找
    var result *Region
    rr.regions.AscendGreaterOrEqual(&regionItem{key: key}, func(i btree.Item) bool {
        r := i.(*regionItem).region
        if bytes.Compare(key, r.StartKey) >= 0 &&
           bytes.Compare(key, r.EndKey) < 0 {
            result = r
            return false  // 停止遍历
        }
        return true
    })

    if result != nil {
        rr.cache.Add(string(key), result)
    }

    return result, nil
}
```

### 3.3 Region分裂与合并

**Region分裂流程：**

```go
// SplitChecker 检查是否需要分裂
type SplitChecker struct {
    // 分裂阈值
    MaxSize       uint64  // 最大大小（默认96MB）
    MaxKeys       uint64  // 最大键数量
    MaxWriteBytes uint64  // 每秒写入字节数阈值
}

// 执行分裂
func (rg *RaftGroup) proposeSplit(splitKey []byte) error {
    // 1. 检查分裂条件
    if rg.Region.Size < rg.splitChecker.MaxSize {
        return errors.New("region size too small")
    }

    // 2. 构造分裂提案
    splitReq := &SplitRequest{
        SplitKey:    splitKey,
        NewRegionID: generateRegionID(),
        NewPeerIDs:  generatePeerIDs(len(rg.Region.Peers)),
    }

    // 3. 提交分裂命令到Raft
    proposal := &Proposal{
        CmdType: CmdTypeAdmin,
        AdminRequest: &AdminRequest{
            CmdType: AdminCmdTypeSplit,
            Split:   splitReq,
        },
    }

    return rg.propose(proposal)
}

// 分裂后的Region状态
func applySplit(parentRegion *Region, splitReq *SplitRequest) (*Region, *Region) {
    // 父Region保持左半部分
    leftRegion := &Region{
        ID:          parentRegion.ID,
        StartKey:    parentRegion.StartKey,
        EndKey:      splitReq.SplitKey,
        RegionEpoch: &RegionEpoch{
            ConfVer: parentRegion.RegionEpoch.ConfVer,
            Version: parentRegion.RegionEpoch.Version + 1,
        },
        Peers: parentRegion.Peers,
    }

    // 新Region为右半部分
    rightRegion := &Region{
        ID:          splitReq.NewRegionID,
        StartKey:    splitReq.SplitKey,
        EndKey:      parentRegion.EndKey,
        RegionEpoch: &RegionEpoch{
            ConfVer: parentRegion.RegionEpoch.ConfVer,
            Version: 1,
        },
        Peers: createPeers(splitReq.NewPeerIDs),
    }

    return leftRegion, rightRegion
}
```

---

## 四、领导均衡

### 4.1 领导分布问题

```
不均衡场景：
┌─────────┐    ┌─────────┐    ┌─────────┐
│ Node A  │    │ Node B  │    │ Node C  │
│  Raft-1 │    │         │    │         │
│  (L)    │    │         │    │         │
│  Raft-2 │    │  Raft-2 │    │  Raft-2 │
│  (L)    │    │  (F)    │    │  (F)    │
│  Raft-3 │    │  Raft-3 │    │  Raft-3 │
│  (L)    │    │  (F)    │    │  (F)    │
│  Raft-4 │    │  Raft-4 │    │  Raft-4 │
│  (L)    │    │  (F)    │    │  (F)    │
└─────────┘    └─────────┘    └─────────┘
Node A: 100% Leader负载   Node B/C: 0% Leader负载 ❌
```

### 4.2 领导均衡调度器

```go
// LeaderBalancer 领导均衡调度器
type LeaderBalancer struct {
    multiRaft   *MultiRaft
    storeStats  map[uint64]*StoreStats  // StoreID -> 统计

    // 均衡阈值
    maxDiffRatio float64  // 最大差异比例（默认0.05）
    minBalanceInterval time.Duration
}

type StoreStats struct {
    StoreID       uint64
    LeaderCount   int
    RegionCount   int
    LeaderSize    int64
    RegionSize    int64

    // 资源使用率
    CPUUsage      float64
    DiskUsage     float64
    IOUtil        float64
}

// 计算均衡计划
func (lb *LeaderBalancer) calculateBalance() []*TransferLeaderOperator {
    var operators []*TransferLeaderOperator

    // 计算每个节点的期望Leader数量
    totalLeaders := 0
    totalRegions := 0
    for _, stats := range lb.storeStats {
        totalLeaders += stats.LeaderCount
        totalRegions += stats.RegionCount
    }
    avgLeaders := float64(totalLeaders) / float64(len(lb.storeStats))

    // 找出Leader过多的节点和过少的节点
    var sourceStores, targetStores []*StoreStats
    for _, stats := range lb.storeStats {
        diff := float64(stats.LeaderCount) - avgLeaders
        if diff > avgLeaders*lb.maxDiffRatio {
            sourceStores = append(sourceStores, stats)
        } else if diff < -avgLeaders*lb.maxDiffRatio {
            targetStores = append(targetStores, stats)
        }
    }

    // 生成Leader转移操作
    for _, source := range sourceStores {
        for source.LeaderCount > int(avgLeaders*(1+lb.maxDiffRatio)) {
            // 选择合适的Raft组和目标节点
            region, target := lb.selectBestTransfer(source, targetStores)
            if region == nil {
                break
            }

            operators = append(operators, &TransferLeaderOperator{
                RegionID:   region.ID,
                FromStore:  source.StoreID,
                ToStore:    target.StoreID,
                ToPeer:     region.getPeer(target.StoreID),
            })

            source.LeaderCount--
            target.LeaderCount++
        }
    }

    return operators
}
```

### 4.3 领导转移实现

```go
// 安全地转移Leader
type TransferLeaderOperator struct {
    RegionID  uint64
    FromStore uint64
    ToStore   uint64
    ToPeer    *Peer
}

func (rg *RaftGroup) transferLeader(toPeer *Peer) error {
    // 1. 检查前提条件
    if rg.RaftState.Status().Lead != rg.NodeID {
        return errors.New("not leader")
    }

    // 2. 检查目标节点日志同步状态
    progress := rg.RaftState.Progress()[toPeer.ID]
    if progress == nil {
        return errors.New("peer not found in progress")
    }
    if progress.Match < rg.RaftState.LastIndex()-100 {
        return errors.New("target peer lag too much")
    }

    // 3. 发送TransferLeader消息
    msg := raftpb.Message{
        Type: raftpb.MsgTransferLeader,
        To:   toPeer.ID,
        From: rg.NodeID,
    }

    return rg.RaftState.Step(msg)
}

// 使用Lease Read确保转移期间不丢读请求
type LeaseRead struct {
    validUntil time.Time
    leaderTerm uint64
}

func (lr *LeaseRead) isValid(now time.Time, currentTerm uint64) bool {
    return now.Before(lr.validUntil) && lr.leaderTerm == currentTerm
}
```

---

## 五、成员变更

### 5.1 Joint Consensus成员变更

Raft原始论文中的单次成员变更存在安全性问题，TiKV采用了**Joint Consensus**（联合共识）方案。

```go
// Joint Consensus状态机
type ConfChangeState int

const (
    ConfChangeStateNone ConfChangeState = iota
    ConfChangeStateEnteringJoint  // 进入Joint Consensus
    ConfChangeStateJoint          // Joint Consensus阶段
    ConfChangeStateLeavingJoint   // 离开Joint Consensus
)

// 成员变更配置
type ConfChangeV2 struct {
    Transition ConfChangeTransitionType
    Changes    []ConfChangeSingle
}

type ConfChangeSingle struct {
    Type    ConfChangeType  // AddNode, RemoveNode, UpdateNode
    NodeID  uint64
    Context []byte  // 额外信息（如Learner标志）
}
```

### 5.2 成员变更流程

```
初始配置 C_old      Joint阶段 C_old ∪ C_new      新配置 C_new
┌─────────┐        ┌─────────────────┐        ┌─────────┐
│ Node A  │        │ Node A (Joint)  │        │ Node A  │
│ Node B  │   ──▶  │ Node B (Joint)  │   ──▶  │ Node B  │
│ Node C  │        │ Node C (Joint)  │        │ Node D  │
│         │        │ Node D (Joint)  │        │ Node E  │
└─────────┘        └─────────────────┘        └─────────┘
   Cold=3               Cold∪Cnew=5               Cnew=3
 需要2票               需要3票(Cold)+3票(Cnew)    需要2票
```

```go
// 应用成员变更
func (rg *RaftGroup) applyConfChangeV2(cc *ConfChangeV2) error {
    switch cc.Transition {
    case ConfChangeTransitionAuto:
        // 自动判断是否需要Joint Consensus
        if len(cc.Changes) > 1 {
            // 多节点变更需要Joint Consensus
            return rg.enterJointConsensus(cc.Changes)
        } else {
            // 单节点变更可以直接应用
            return rg.applySingleChange(cc.Changes[0])
        }

    case ConfChangeTransitionJointImplicit:
        // 强制使用Joint Consensus
        return rg.enterJointConsensus(cc.Changes)

    case ConfChangeTransitionJointExplicit:
        // 退出Joint Consensus
        return rg.leaveJointConsensus()
    }

    return nil
}

// 进入Joint Consensus
func (rg *RaftGroup) enterJointConsensus(changes []ConfChangeSingle) error {
    // 1. 构造Joint Configuration
    jointConfig := &Configuration{
        Voters:    make(map[uint64]struct{}),
        Learners:  make(map[uint64]struct{}),
        VotersOutgoing: make(map[uint64]struct{}),  // Cold成员
    }

    // 复制当前配置到Outgoing
    for id := range rg.config.Voters {
        jointConfig.VotersOutgoing[id] = struct{}{}
    }

    // 应用变更得到新配置
    for _, change := range changes {
        switch change.Type {
        case ConfChangeAddNode:
            jointConfig.Voters[change.NodeID] = struct{}{}
        case ConfChangeAddLearnerNode:
            jointConfig.Learners[change.NodeID] = struct{}{}
        case ConfChangeRemoveNode:
            delete(jointConfig.VotersOutgoing, change.NodeID)
            delete(jointConfig.Voters, change.NodeID)
        }
    }

    // 2. 进入Joint Consensus状态
    rg.config = jointConfig
    rg.confState.JointConsensus = true

    // 3. 持久化新配置
    return rg.persistConfState()
}

// 退出Joint Consensus
func (rg *RaftGroup) leaveJointConsensus() error {
    // 1. 清理Outgoing配置
    finalConfig := &Configuration{
        Voters:   rg.config.Voters,
        Learners: rg.config.Learners,
    }

    // 2. 更新为最终配置
    rg.config = finalConfig
    rg.confState.JointConsensus = false

    // 3. 处理被移除的节点
    rg.pruneRemovedNodes()

    return rg.persistConfState()
}
```

### 5.3 Learner节点

```go
// Learner角色用于新节点加入时的数据同步
type LearnerNode struct {
    Peer

    // 同步状态
    SnapshotInProgress bool
    LastIndexSynced    uint64
    SyncProgress       float64
}

// 将Learner提升为Voter
func (rg *RaftGroup) promoteLearner(learnerID uint64) error {
    // 1. 检查Learner是否已同步
    learner := rg.getPeer(learnerID)
    if learner == nil || !learner.IsLearner {
        return errors.New("not a learner")
    }

    progress := rg.RaftState.Progress()[learnerID]
    if progress.Match < rg.RaftState.LastIndex() {
        return errors.New("learner not fully synced")
    }

    // 2. 提交成员变更（Learner -> Voter）
    cc := &ConfChangeV2{
        Changes: []ConfChangeSingle{
            {
                Type:   ConfChangeUpdateNode,
                NodeID: learnerID,
            },
        },
    }

    return rg.proposeConfChange(cc)
}
```

---

## 六、TiKV实现参考

### 6.1 TiKV架构概览

```
┌─────────────────────────────────────────────────────────────┐
│                         TiKV Node                            │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                    gRPC Service                      │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  │   │
│  │  │   KV API    │  │  Raw API    │  │  Txn API    │  │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘  │   │
│  └─────────────────────────────────────────────────────┘   │
│                         │                                    │
│  ┌──────────────────────┼───────────────────────────────┐   │
│  │                 RaftStore                           │   │
│  │  ┌───────────────────┴───────────────────────────┐   │   │
│  │  │              Multi-Raft 层                    │   │   │
│  │  │  ┌─────────┐ ┌─────────┐ ┌─────────┐          │   │   │
│  │  │  │Peer 1   │ │Peer 2   │ │Peer N   │ ...      │   │   │
│  │  │  │(Raft)   │ │(Raft)   │ │(Raft)   │          │   │   │
│  │  │  └─────────┘ └─────────┘ └─────────┘          │   │   │
│  │  └───────────────────────────────────────────────┘   │   │
│  └───────────────────────────────────────────────────────┘   │
│                         │                                    │
│  ┌──────────────────────┼───────────────────────────────┐   │
│  │              Storage Engine (RocksDB)                 │   │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐              │   │
│  │  │  RaftDB │  │  KV DB  │  │ Lock CF │              │   │
│  │  │(WAL)    │  │(数据)   │  │(事务锁)  │              │   │
│  │  └─────────┘  └─────────┘  └─────────┘              │   │
│  └───────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

### 6.2 TiKV关键代码结构

```rust
// raftstore/src/store/peer.rs - TiKV的RaftGroup封装
pub struct Peer {
    pub peer: metapb::Peer,
    pub region_id: u64,
    pub raft_group: RawNode<PeerStorage>,

    // 提案队列
    proposals: ProposalQueue,

    // 应用状态
    apply_state: ApplyState,

    // 读取队列优化
    pending_reads: ReadIndexQueue,

    // 副本状态跟踪
    peer_cache: HashMap<u64, metapb::Peer>,
}

impl Peer {
    // 处理Raft Ready
    pub fn handle_raft_ready<T: Transport>(
        &mut self,
        ctx: &mut PollContext<T>,
    ) -> Option<ReadyResult> {
        if !self.raft_group.has_ready() {
            return None;
        }

        let mut ready = self.raft_group.ready_since(self.last_applying_idx);

        // 1. 发送Raft消息
        self.send_raft_messages(ctx, &ready.messages);

        // 2. 持久化日志
        if !ready.entries().is_empty() {
            self.append_entries(ctx, ready.take_entries());
        }

        // 3. 处理快照
        if let Some(snapshot) = ready.snapshot() {
            self.apply_snapshot(ctx, snapshot);
        }

        // 4. 应用已提交条目
        if !ready.committed_entries().is_empty() {
            self.raft_group.advance_apply(
                ready.committed_entries().last().unwrap().index
            );
        }

        // 5. 更新HardState
        if let Some(hs) = ready.hs() {
            self.save_raft_state(hs);
        }

        Some(ReadyResult {
            committed_entries: ready.take_committed_entries(),
        })
    }
}
```

### 6.3 Local Reader优化

```rust
// 使用Lease Read避免Raft日志开销
pub struct LocalReader {
    store_id: u64,
    delegates: HashMap<u64, ReadDelegate>,
}

pub struct ReadDelegate {
    region: metapb::Region,
    leader_lease: RemoteLease,
    last_valid_ts: Instant,
    applied_index_term: u64,
    applied_index: u64,
}

impl LocalReader {
    pub fn read(&self, req: RaftCmdRequest) -> Result<RaftCmdResponse> {
        let region_id = req.get_header().get_region_id();
        let delegate = self.delegates.get(&region_id)
            .ok_or(Error::RegionNotFound(region_id))?;

        // 检查Leader Lease是否有效
        if !delegate.leader_lease.inspect(None).is_satisfied() {
            // Lease过期，转发给Leader
            return Err(Error::NotLeader(region_id, None));
        }

        // 检查Region Epoch
        let req_epoch = req.get_header().get_region_epoch();
        if util::is_epoch_stale(delegate.region.get_region_epoch(), req_epoch) {
            return Err(Error::EpochNotMatch(
                format!("current epoch of region {:?} is {:?}",
                    region_id, delegate.region.get_region_epoch()),
                vec![delegate.region.clone()],
            ));
        }

        // 本地执行读请求
        self.execute_read(delegate, req)
    }
}
```

### 6.4 Multi-Raft性能优化总结

| 优化技术 | 说明 | 效果 |
|----------|------|------|
| **Batch处理** | 批量聚合多个Raft组的Ready | 减少I/O次数，提升吞吐 |
| **Pipeline** | 并行处理日志持久化和网络发送 | 降低延迟 |
| **Leader Lease** | Leader租约机制支持本地读 | 读性能提升10x+ |
| **Follower Read** | 支持从Follower读取数据 | 读负载均衡 |
| **异步Apply** | 日志提交和应用异步执行 | 降低写延迟 |
| **Region缓存** | 缓存Region路由信息 | 减少路由查找开销 |

---

## 参考资料

1. [Raft论文](https://raft.github.io/raft.pdf) - Diego Ongaro, John Ousterhout
2. [TiKV源码](https://github.com/tikv/tikv) - TiKV GitHub仓库
3. [TiKV设计与实现](https://tikv.org/deep-dive/) - TiKV官方文档
4. [etcd Raft库](https://github.com/etcd-io/raft) - Go语言Raft实现

> **状态**: ✅ 已完成
