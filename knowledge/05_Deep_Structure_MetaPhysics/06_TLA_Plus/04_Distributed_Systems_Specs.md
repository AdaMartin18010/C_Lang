# 分布式系统规范案例

## 1. 分布式系统概述

### 1.1 分布式系统挑战

```
分布式系统的核心挑战 (FLP不可能性):
┌─────────────────────────────────────────────────────────┐
│ 异步网络环境中，即使只有一个故障进程，也不存在          │
│ 确定性的共识算法能保证在有限时间内达成一致。            │
└─────────────────────────────────────────────────────────┘

主要挑战:
├── 网络分区 (Network Partition)
│   └── 节点间通信中断，形成脑裂
├── 消息丢失/延迟/乱序
│   └── TCP保证顺序，但UDP不保证
├── 进程故障 (Crash/Byzantine)
│   ├── 崩溃停止 (Crash-stop)
│   ├── 崩溃恢复 (Crash-recovery)
│   └── 拜占庭故障 (Byzantine)
├── 时钟同步问题
│   └── 没有全局时钟，只有本地时钟
└── 并发执行的不确定性
    └── 非确定性交错执行

共识问题三元组 (CAP定理):
┌───────────────┬─────────────────────────────────────┐
│ Consistency   │ 所有节点看到相同的数据              │
│ Availability  │ 每个请求都得到响应                  │
│ Partition     │ 系统在网络分区时继续运行            │
└───────────────┴─────────────────────────────────────┘
注意: 网络分区时，只能保证C或A中的一个
```

### 1.2 TLA+验证价值

```
为什么用TLA+验证分布式系统?
┌─────────────────────────────────────────────────────────┐
│ 1. 在设计阶段发现bug，而非生产环境                       │
│ 2. 穷尽所有可能的执行序列                                │
│ 3. 验证边界情况和异常场景                                │
│ 4. 提供形式化正确性保证                                  │
│ 5. 作为精确的文档                                        │
└─────────────────────────────────────────────────────────┘

工业应用案例:
├── Amazon: DynamoDB, S3, EBS
├── Microsoft: CosmosDB, Azure Storage
├── MongoDB: 复制协议验证
├── Apache: Zookeeper改进
└── CockroachDB: 分布式事务
```

---

## 2. Raft共识算法

### 2.1 Raft算法概述

```
Raft核心设计原则: 可理解性优先于性能

Raft角色:
┌─────────────┬──────────────────────────────────────────┐
│ Leader      │ 处理所有客户端请求，管理日志复制         │
│ Follower    │ 被动响应Leader和Candidate的请求          │
│ Candidate   │ 选举期间的角色，争取成为Leader           │
└─────────────┴──────────────────────────────────────────┘

Raft两大核心问题:
┌──────────────────┐     ┌──────────────────┐
│   Leader选举     │     │   日志复制       │
│                  │     │                  │
│ - 超时机制       │     │ - 日志条目追加   │
│ - 心跳保持       │     │ - 一致性检查     │
│ - 多数派投票     │     │ - 提交规则       │
└──────────────────┘     └──────────────────┘

状态机安全保证:
┌──────────────────────────────────────────────────────────┐
│ 1. 选举安全: 一个任期内最多一个Leader                      │
│ 2. Leader完备: Leader包含所有已提交条目                    │
│ 3. 状态机安全: 已提交条目不会被覆盖                        │
│ 4. 领导人只追加: Leader不会删除或修改日志                  │
│ 5. 提交规则: 条目在当前任期被多数复制才算提交              │
└──────────────────────────────────────────────────────────┘
```

### 2.2 Raft TLA+完整规范

```tla
---- MODULE Raft ----
EXTENDS Naturals, FiniteSets, Sequences, TLC

CONSTANTS
  Servers,           (* 服务器集合 *)
  MaxLogLen,         (* 最大日志长度，用于限制状态空间 *)
  MaxTerm,           (* 最大任期 *)
  MaxClientRequests  (* 最大客户端请求数 *)

ASSUME /\ Servers /= {}
       /\ MaxLogLen \in Nat
       /\ MaxTerm \in Nat

(* 服务器角色 *)
Follower == "follower"
Candidate == "candidate"
Leader == "leader"
ServerRoles == {Follower, Candidate, Leader}

(* 消息类型定义 *)
RequestVoteRequest == [type: {"RequestVoteRequest"},
                       term: Nat,
                       candidate: Servers,
                       lastLogIndex: Nat,
                       lastLogTerm: Nat]

RequestVoteResponse == [type: {"RequestVoteResponse"},
                        term: Nat,
                        voter: Servers,
                        voteGranted: BOOLEAN]

AppendEntriesRequest == [type: {"AppendEntriesRequest"},
                         term: Nat,
                         leader: Servers,
                         prevLogIndex: Nat,
                         prevLogTerm: Nat,
                         entries: Seq(Nat),
                         leaderCommit: Nat]

AppendEntriesResponse == [type: {"AppendEntriesResponse"},
                          term: Nat,
                          follower: Servers,
                          success: BOOLEAN,
                          matchIndex: Nat]

Message == RequestVoteRequest \union RequestVoteResponse
           \union AppendEntriesRequest \union AppendEntriesResponse

(* --algorithm Raft
variables
  (* 持久状态（所有服务器） *)
  currentTerm = [s \in Servers |-> 0],
  votedFor = [s \in Servers |-> Nil],  (* Nil表示未投票 *)
  log = [s \in Servers |-> << >>],

  (* 易失状态（所有服务器） *)
  commitIndex = [s \in Servers |-> 0],
  lastApplied = [s \in Servers |-> 0],

  (* 易失状态（Leader特有） *)
  nextIndex = [s \in Servers |-> [t \in Servers \ {s} |-> 1]],
  matchIndex = [s \in Servers |-> [t \in Servers \ {s} |-> 0]],

  (* 服务器角色 *)
  role = [s \in Servers |-> Follower],

  (* 消息网络 *)
  messages = {},

  (* 客户端请求计数 *)
  clientRequestCount = 0;

(* 辅助函数 *)
define
  Nil == 0

  (* 获取日志条目任期，0表示不存在 *)
  LogTerm(s, i) ==
    IF i = 0 THEN 0
    ELSE IF i > Len(log[s]) THEN 0
    ELSE log[s][i]

  (* 最后日志索引和任期 *)
  LastLogIndex(s) == Len(log[s])
  LastLogTerm(s) == LogTerm(s, LastLogIndex(s))

  (* 多数派 *)
  Quorum == {Q \in SUBSET Servers : Cardinality(Q) * 2 > Cardinality(Servers)}

  (* 已提交条目集 *)
  CommittedEntries ==
    {<<i, t>> : i \in 1..MaxLogLen, t \in 0..MaxTerm :
      \E Q \in Quorum :
        \A s \in Q : /\ Len(log[s]) >= i
                     /\ log[s][i] = t
                     /\ commitIndex[s] >= i}

  (* ===== 不变式 ===== *)

  (* 类型不变式 *)
  TypeInvariant ==
    /\ currentTerm \in [Servers -> 0..MaxTerm]
    /\ votedFor \in [Servers -> Servers \union {Nil}]
    /\ log \in [Servers -> Seq(0..MaxTerm)]
    /\ commitIndex \in [Servers -> 0..MaxLogLen]
    /\ role \in [Servers -> ServerRoles]
    /\ messages \subseteq Message

  (* 任期单调性 *)
  TermMonotonicity ==
    \A s \in Servers :
      \A i, j \in DOMAIN log[s] :
        i <= j => LogTerm(s, i) <= LogTerm(s, j)

  (* 日志匹配: 如果两个日志在相同索引有相同任期，则前面内容相同 *)
  LogMatching ==
    \A s1, s2 \in Servers :
      \A i \in 1..Min(Len(log[s1]), Len(log[s2])) :
        LogTerm(s1, i) = LogTerm(s2, i) =>
          \A j \in 1..i : log[s1][j] = log[s2][j]

  (* 状态机安全: 已提交条目不会被覆盖 *)
  StateMachineSafety ==
    \A s1, s2 \in Servers :
      \A i \in 1..commitIndex[s1] :
        i <= commitIndex[s2] => log[s1][i] = log[s2][i]

  (* Leader完备性 *)
  LeaderCompleteness ==
    \A s \in Servers :
      role[s] = Leader =>
        \A t \in Servers, i \in 1..commitIndex[t] :
          i <= Len(log[s]) => log[s][i] = log[t][i]

  (* 选举安全: 一个任期内最多一个Leader *)
  ElectionSafety ==
    \A s1, s2 \in Servers :
      (role[s1] = Leader /\ role[s2] = Leader /\
       currentTerm[s1] = currentTerm[s2]) => s1 = s2

  (* 提交规则正确性 *)
  CommitCorrectness ==
    \A s \in Servers :
      commitIndex[s] <= Len(log[s])
end define

(* 发送消息 *)
macro Send(m) begin
  messages := messages \union {m};
end macro;

(* 转换为Candidate *)
macro BecomeCandidate(s) begin
  role[s] := Candidate;
  currentTerm[s] := currentTerm[s] + 1;
  votedFor[s] := s;

  (* 为自己投票后广播RequestVote *)
  with dest \in Servers \ {s} do
    Send([type |-> "RequestVoteRequest",
          term |-> currentTerm[s],
          candidate |-> s,
          lastLogIndex |-> LastLogIndex(s),
          lastLogTerm |-> LastLogTerm(s)]);
  end with;
end macro;

(* 成为Leader *)
macro BecomeLeader(s) begin
  role[s] := Leader;
  nextIndex[s] := [t \in Servers \ {s} |-> LastLogIndex(s) + 1];
  matchIndex[s] := [t \in Servers \ {s} |-> 0];
end macro;

process Server \in Servers
variables
  votesGranted = {},
  electionTimer = 0
begin
ServerLoop:
  while TRUE do

    (* ===== Follower行为 ===== *)
    either
      await role[self] = Follower;

      (* 处理RequestVoteRequest *)
      with m \in {msg \in messages :
                   /\ msg.type = "RequestVoteRequest"
                   /\ msg \notin handledVotes} do

        if m.term > currentTerm[self] then
          currentTerm[self] := m.term;
          votedFor[self] := Nil;
        end if;

        if m.term = currentTerm[self] /\
           (votedFor[self] = Nil \/ votedFor[self] = m.candidate) /\
           (m.lastLogTerm > LastLogTerm(self) \/
            (m.lastLogTerm = LastLogTerm(self) /\
             m.lastLogIndex >= LastLogIndex(self))) then

          votedFor[self] := m.candidate;
          Send([type |-> "RequestVoteResponse",
                term |-> currentTerm[self],
                voter |-> self,
                voteGranted |-> TRUE]);
        else
          Send([type |-> "RequestVoteResponse",
                term |-> currentTerm[self],
                voter |-> self,
                voteGranted |-> FALSE]);
        end if;
      end with;

    (* 处理AppendEntriesRequest *)
    or
      await role[self] = Follower;

      with m \in {msg \in messages : msg.type = "AppendEntriesRequest"} do

        if m.term > currentTerm[self] then
          currentTerm[self] := m.term;
          votedFor[self] := Nil;
        end if;

        if m.term < currentTerm[self] then
          (* 拒绝旧任期Leader *)
          Send([type |-> "AppendEntriesResponse",
                term |-> currentTerm[self],
                follower |-> self,
                success |-> FALSE,
                matchIndex |-> 0]);
        else
          (* 重置选举定时器 *)
          electionTimer := 0;

          (* 日志一致性检查 *)
          if m.prevLogIndex = 0 \/
             (m.prevLogIndex <= Len(log[self]) /\
              LogTerm(self, m.prevLogIndex) = m.prevLogTerm) then

            (* 追加条目 *)
            log[self] :=
              IF m.prevLogIndex >= Len(log[self])
              THEN log[self] \o m.entries
              ELSE SubSeq(log[self], 1, m.prevLogIndex) \o m.entries;

            (* 更新commitIndex *)
            if m.leaderCommit > commitIndex[self] then
              commitIndex[self] := Min(m.leaderCommit, Len(log[self]));
            end if;

            Send([type |-> "AppendEntriesResponse",
                  term |-> currentTerm[self],
                  follower |-> self,
                  success |-> TRUE,
                  matchIndex |-> m.prevLogIndex + Len(m.entries)]);
          else
            (* 日志不匹配 *)
            Send([type |-> "AppendEntriesResponse",
                  term |-> currentTerm[self],
                  follower |-> self,
                  success |-> FALSE,
                  matchIndex |-> 0]);
          end if;
        end if;
      end with;

    (* 选举超时，转换为Candidate *)
    or
      await role[self] = Follower;
      await electionTimer > Timeout;

      if currentTerm[self] < MaxTerm then
        BecomeCandidate(self);
        votesGranted := {self};
        electionTimer := 0;
      end if;

    (* ===== Candidate行为 ===== *)
    (* 处理RequestVoteResponse *)
    or
      await role[self] = Candidate;

      with m \in {msg \in messages :
                   /\ msg.type = "RequestVoteResponse"
                   /\ msg.term = currentTerm[self]
                   /\ msg.voter \notin votesGranted} do

        if m.voteGranted then
          votesGranted := votesGranted \union {m.voter};

          (* 获得多数票，成为Leader *)
          if votesGranted \in Quorum then
            BecomeLeader(self);
          end if;
        end if;
      end with;

    (* 发现更高任期 *)
    or
      await role[self] = Candidate;

      with m \in messages do
        if m.term > currentTerm[self] then
          currentTerm[self] := m.term;
          role[self] := Follower;
          votedFor[self] := Nil;
          votesGranted := {};
        end if;
      end with;

    (* 选举超时 *)
    or
      await role[self] = Candidate;
      await electionTimer > Timeout;

      (* 开始新一轮选举 *)
      if currentTerm[self] < MaxTerm then
        BecomeCandidate(self);
        votesGranted := {self};
        electionTimer := 0;
      end if;

    (* ===== Leader行为 ===== *)
    (* 发送心跳/日志复制 *)
    or
      await role[self] = Leader;

      with follower \in Servers \ {self} do
        prevIdx := nextIndex[self][follower] - 1;
        prevTerm := LogTerm(self, prevIdx);

        Send([type |-> "AppendEntriesRequest",
              term |-> currentTerm[self],
              leader |-> self,
              prevLogIndex |-> prevIdx,
              prevLogTerm |-> prevTerm,
              entries |-> SubSeq(log[self], prevIdx + 1, Len(log[self])),
              leaderCommit |-> commitIndex[self]]);
      end with;

    (* 处理AppendEntriesResponse *)
    or
      await role[self] = Leader;

      with m \in {msg \in messages :
                   /\ msg.type = "AppendEntriesResponse"
                   /\ msg.term = currentTerm[self]} do

        if m.success then
          (* 更新匹配索引 *)
          matchIndex[self][m.follower] := m.matchIndex;
          nextIndex[self][m.follower] := m.matchIndex + 1;

          (* 检查是否可以提交 *)
          with N \in {i \in 1..Len(log[self]) :
                      /\ i > commitIndex[self]
                      /\ LogTerm(self, i) = currentTerm[self]} do
            if {s \in Servers : matchIndex[self][s] >= N} \in Quorum then
              commitIndex[self] := N;
            end if;
          end with;
        else
          (* 日志不匹配，回退nextIndex *)
          if nextIndex[self][m.follower] > 1 then
            nextIndex[self][m.follower] := nextIndex[self][m.follower] - 1;
          end if;
        end if;
      end with;

    (* 发现更高任期 *)
    or
      await role[self] = Leader;

      with m \in messages do
        if m.term > currentTerm[self] then
          currentTerm[self] := m.term;
          role[self] := Follower;
          votedFor[self] := Nil;
        end if;
      end with;

    (* 处理客户端请求 *)
    or
      await role[self] = Leader;
      await clientRequestCount < MaxClientRequests;
      await Len(log[self]) < MaxLogLen;

      log[self] := Append(log[self], currentTerm[self]);
      clientRequestCount := clientRequestCount + 1;

    end either;

    (* 增加选举定时器 *)
    electionTimer := electionTimer + 1;
  end while;
end process;

end algorithm *)
====
```

### 2.3 Raft状态机图

```
Raft服务器状态转换:
                    ┌──────────────────────────────────────┐
                    │                                      │
                    ▼                                      │
┌──────────┐   超时   ┌──────────┐   赢得选举   ┌─────────┐ │
│ Follower │ ───────> │ Candidate│ ───────────> │ Leader  │─┘
└──────────┘          └──────────┘              └─────────┘
      ▲                    │   │                      │
      │                    │   │ 发现更高任期         │
      │                    │   └──────────────────────┘
      │                    │
      │                    │ 超时
      │                    ▼
      │               ┌──────────┐
      └───────────────│ Candidate│ (新一轮选举)
         收到有效     └──────────┘
         AppendEntries

消息交互图:
┌──────────┐                    ┌──────────┐
│ Candidate│ --RequestVote----> │ Follower │
│          │ <--VoteGranted---- │          │
│          │                    │          │
│  Leader  │ --AppendEntries--> │ Follower │
│          │ <--Success-------- │          │
└──────────┘                    └──────────┘
```

---

## 3. 两阶段提交 (2PC)

### 3.1 2PC算法概述

```
两阶段提交参与者:
┌──────────────────────────────────────────────────────────┐
│ Transaction Manager (TM): 协调者，决定提交或中止         │
│ Resource Managers (RM): 参与者，管理资源                 │
└──────────────────────────────────────────────────────────┘

两阶段流程:
┌────────────────┐           ┌────────────────┐
│   阶段1: 投票   │           │  阶段2: 决定   │
├────────────────┤           ├────────────────┤
│ 1. TM发送      │           │ 1. 如果所有    │
│    Prepare     │           │    RM投票Yes   │
│    给所有RM    │           │    TM发送Commit│
│                │           │                │
│ 2. RM执行本地  │           │ 2. 如果有      │
│    准备操作    │           │    RM投票No    │
│                │           │    TM发送Abort │
│ 3. RM回复      │           │                │
│    Yes/No      │           │ 3. RM执行      │
│    给TM        │           │    Commit/Abort│
└────────────────┘           └────────────────┘

2PC状态图:
TM状态: init -> preparing -> committed/aborted
RM状态: working -> prepared -> committed/aborted
       working -> aborted
```

### 3.2 2PC完整TLA+规范

```tla
---- MODULE TwoPhaseCommit ----
EXTENDS Naturals, FiniteSets, Sequences, TLC

CONSTANTS
  RM,                (* 资源管理器集合 *)
  TM,                (* 事务管理器 *)
  MaxRounds          (* 最大事务轮数 *)

ASSUME RM /= {}

Messages ==
  [type : {"Prepare"}, tm : {TM}] \union
  [type : {"Prepared", "Abort", "Commit"}, rm : RM] \union
  [type : {"Yes", "No"}, rm : RM]

(* --algorithm TwoPhaseCommit
variables
  (* RM状态 *)
  rmState = [r \in RM |-> "working"],

  (* TM状态 *)
  tmState = "init",

  (* 消息集合 *)
  msgs = {},

  (* 已收到的Prepared消息 *)
  preparedRMs = {},

  (* 事务轮数 *)
  round = 0;

define
  (* ===== 辅助谓词 ===== *)

  (* 可以提交: 所有RM都Prepared *)
  CanCommit ==
    preparedRMs = RM

  (* 可以中止: 有RM拒绝或TM决定中止 *)
  CanAbort ==
    tmState = "aborted" \/
    \E m \in msgs : m.type = "No"

  (* ===== 不变式 ===== *)

  (* 类型不变式 *)
  TypeInvariant ==
    /\ rmState \in [RM -> {"working", "prepared", "committed", "aborted"}]
    /\ tmState \in {"init", "preparing", "committed", "aborted"}
    /\ msgs \subseteq Messages
    /\ preparedRMs \subseteq RM

  (* 安全: 不能同时有提交和中止 *)
  Consistency ==
    ~\E r1, r2 \in RM :
      rmState[r1] = "committed" /\ rmState[r2] = "aborted"

  (* TM决定后的RM一致性 *)
  TMDecisionImpliesRM ==
    /\ (tmState = "committed" =>
        \A r \in RM : rmState[r] \in {"prepared", "committed"})
    /\ (tmState = "aborted" =>
        \A r \in RM : rmState[r] \in {"working", "aborted"})

  (* 提交的前提条件 *)
  CommitPrecondition ==
    tmState = "committed" =>
      \A r \in RM : [type |-> "Prepared", rm |-> r] \in msgs

  (* 一旦提交不能撤销 *)
  CommitIrrevocable ==
    [][tmState = "committed" => tmState' = "committed"]_vars
end define

macro Send(m) begin
  msgs := msgs \union {m};
end macro;

(* 开始新事务 *)
macro StartTransaction begin
  round := round + 1;
  tmState := "init";
  rmState := [r \in RM |-> "working"];
  preparedRMs := {};
  msgs := {};
end macro;

process TMProcess = TM
begin
TMStart:
  while round < MaxRounds do

TMInit:
    (* TM决定开始事务 *)
    tmState := "preparing";

    (* 发送Prepare给所有RM *)
    with r \in RM do
      Send([type |-> "Prepare", tm |-> TM]);
    end with;

TMAwait:
    (* 等待RM响应 *)
    while TRUE do
      either
        (* 收到Prepared消息 *)
        with r \in RM \ preparedRMs do
          await [type |-> "Prepared", rm |-> r] \in msgs;
          preparedRMs := preparedRMs \union {r};

          (* 如果所有都Prepared，发送Commit *)
          if preparedRMs = RM then
            with rm \in RM do
              Send([type |-> "Commit", rm |-> rm]);
            end with;
            tmState := "committed";
            goto TMNextRound;
          end if;
        end with;

      or
        (* 收到No消息或决定中止 *)
        either
          (* 某个RM拒绝 *)
          with r \in RM do
            await [type |-> "No", rm |-> r] \in msgs;
          end with;
        or
          (* TM单方面决定中止 *)
          await TRUE;
        end either;

        (* 发送Abort给所有RM *)
        with rm \in RM do
          Send([type |-> "Abort", rm |-> rm]);
        end with;
        tmState := "aborted";
        goto TMNextRound;

      end either;
    end while;

TMNextRound:
    StartTransaction;
  end while;
end process;

process RMProcess \in RM
begin
RMStart:
  while TRUE do

RMWorking:
    await rmState[self] = "working";

    (* 等待Prepare消息 *)
    await [type |-> "Prepare", tm |-> TM] \in msgs;

RMVote:
    either
      (* 决定Prepared *)
      rmState[self] := "prepared";
      Send([type |-> "Prepared", rm |-> self]);

      (* 等待Commit或Abort *)
      either
        await [type |-> "Commit", rm |-> self] \in msgs;
        rmState[self] := "committed";
      or
        await [type |-> "Abort", rm |-> self] \in msgs;
        rmState[self] := "aborted";
      end either;

    or
      (* 决定Abort *)
      rmState[self] := "aborted";
      Send([type |-> "No", rm |-> self]);
    end either;
  end while;
end process;

end algorithm *)
====
```

### 3.3 2PC存在的问题

```
2PC的阻塞问题:
┌──────────────────────────────────────────────────────────┐
│ 场景: TM在发送Commit后崩溃                               │
│                                                          │
│ TM: sent Commit -> [CRASH]                               │
│ RM1: received Commit -> committed                        │
│ RM2: waiting for Commit -> BLOCKED                      │
│                                                          │
│ 结果: RM2必须等待TM恢复才能知道决定                       │
│       这就是2PC的阻塞特性                                 │
└──────────────────────────────────────────────────────────┘

3PC解决方案:
┌──────────────────────────────────────────────────────────┐
│ 添加预提交阶段，RM在收到PreCommit后知道其他RM都同意了     │
│ 如果RM超时未收到决定，可以与其他RM通信得知结果            │
│ 代价: 额外的消息往返                                      │
└──────────────────────────────────────────────────────────┘
```

---

## 4. Paxos算法概述

### 4.1 Paxos核心概念

```
Paxos角色:
┌──────────────────────────────────────────────────────────┐
│ Proposers: 提出提案 (value)                              │
│ Acceptors: 对提案进行投票                                │
│ Learners: 学习已被选定的值                               │
│                                                          │
│ 注意: 一个进程可以同时担任多个角色                        │
└──────────────────────────────────────────────────────────┘

Paxos两阶段:
┌──────────────────────────────────────────────────────────┐
│ Phase 1 (Prepare):                                       │
│   Proposer: 发送 Prepare(n) 给多数Acceptor               │
│   Acceptor: 如果n > 之前所有Prepare，回复Promise         │
│                                                          │
│ Phase 2 (Accept):                                        │
│   Proposer: 收到多数Promise后，发送 Accept(n, v)         │
│   Acceptor: 如果没有更高n的Prepare，接受v                │
└──────────────────────────────────────────────────────────┘

Paxos保证:
┌──────────────────────────────────────────────────────────┐
│ 安全性:                                                  │
│   - 只有一个值可以被选定                                 │
│   - 只有被提出的值才能被选定                             │
│   - 只有被选定的值才能被学习                             │
│                                                          │
│ 活性 (在多数派存活时):                                   │
│   - 最终会有值被选定                                     │
│   - 进程最终会学习被选定的值                             │
└──────────────────────────────────────────────────────────┘
```

### 4.2 Paxos vs Raft对比

```
┌────────────────┬───────────────────┬───────────────────┐
│ 特性           │ Paxos             │ Raft              │
├────────────────┼───────────────────┼───────────────────┤
│ 设计目标       │ 理论正确性        │ 可理解性          │
│ 领导者         │ 无固定Leader      │ 强Leader模型      │
│ 日志复制       │ 单值共识          │ 连续日志复制      │
│ 成员变更       │ 复杂              │ 联合共识          │
│ 实现复杂度     │ 高                │ 中                │
│ 性能           │ 需要优化          │ 良好              │
│ 工业应用       │ Chubby, ZooKeeper │ etcd, Consul      │
└────────────────┴───────────────────┴───────────────────┘

关系: Raft是Multi-Paxos的工程化、可理解版本
```

---

## 5. 与C语言Raft实现的关联

### 5.1 C实现映射关系

```
TLA+规范到C实现的映射:

TLA+                           C语言实现
─────────────────────────────────────────────────────────────
VARIABLE currentTerm      -->  struct RaftState {
                                uint64_t current_term;
                              }

VARIABLE log              -->  struct LogEntry {
                                uint64_t term;
                                char* command;
                              } *log;

VARIABLE role             -->  enum Role {
                                FOLLOWER, CANDIDATE, LEADER
                              } role;

process Server            -->  void* raft_server_thread(void* arg)

Send(m)                   -->  int send_message(int sock,
                                               Message* msg)

await condition           -->  while (!condition) {
                                pthread_cond_wait(&cond, &mutex);
                              }

messages set              -->  MessageQueue incoming;
                               MessageQueue outgoing;

macro                     -->  static inline function

procedure                 -->  regular function
```

### 5.2 验证指导实现

```c
/*
 * 基于TLA+规范实现的Raft互斥锁保护
 * 对应TLA+中的类型不变式和状态转换
 */

#include <pthread.h>
#include <stdatomic.h>

/* 对应TLA+ TypeInvariant */
struct RaftServer {
    _Atomic uint64_t current_term;
    _Atomic uint32_t role;  /* 0=FOLLOWER, 1=CANDIDATE, 2=LEADER */

    /* 对应TLA+ log */
    struct LogEntry *log;
    size_t log_len;
    pthread_rwlock_t log_lock;

    /* 对应TLA+ commitIndex, lastApplied */
    _Atomic uint64_t commit_index;
    _Atomic uint64_t last_applied;

    /* Leader特有状态 - 对应TLA+ nextIndex, matchIndex */
    uint64_t *next_index;
    uint64_t *match_index;

    /* 选举定时器 */
    pthread_mutex_t timer_mutex;
    pthread_cond_t timer_cond;
    uint64_t election_timeout;

    /* 投票状态 - 对应TLA+ votedFor */
    _Atomic uint32_t voted_for;
};

/* 对应TLA+ ElectionSafety: 任期检查确保唯一Leader */
int become_leader(struct RaftServer *s) {
    uint64_t term = atomic_load(&s->current_term);

    /* 原子性地检查和更新角色 */
    uint32_t expected = CANDIDATE;
    if (!atomic_compare_exchange_strong(&s->role,
                                         &expected,
                                         LEADER)) {
        return -1;  /* 已经不是Candidate了 */
    }

    /* 初始化Leader状态 - 对应TLA+ BecomeLeader */
    pthread_rwlock_wrlock(&s->log_lock);
    for (int i = 0; i < num_servers; i++) {
        if (i != self_id) {
            s->next_index[i] = s->log_len + 1;
            s->match_index[i] = 0;
        }
    }
    pthread_rwlock_unlock(&s->log_lock);

    return 0;
}

/* 对应TLA+ StateMachineSafety: 日志一致性检查 */
int append_entries(struct RaftServer *s,
                   uint64_t prev_log_index,
                   uint64_t prev_log_term,
                   struct LogEntry *entries,
                   size_t n_entries) {
    pthread_rwlock_wrlock(&s->log_lock);

    /* 日志一致性检查 - 对应TLA+ LogMatching */
    if (prev_log_index > 0) {
        if (prev_log_index > s->log_len ||
            s->log[prev_log_index - 1].term != prev_log_term) {
            pthread_rwlock_unlock(&s->log_lock);
            return -1;  /* 日志不匹配 */
        }
    }

    /* Leader只追加 - 对应TLA+ LeaderAppendOnly */
    /* 不删除已存在的条目，只覆盖冲突的 */

    pthread_rwlock_unlock(&s->log_lock);
    return 0;
}
```

### 5.3 常见实现bug与TLA+发现

```
从TLA+规范发现的常见C实现bug:

1. 任期处理不一致
   TLA+发现: 收到旧任期消息时没有正确更新
   C代码: 缺少 current_term = max(current_term, msg->term)

2. 日志索引越界
   TLA+发现: TypeInvariant 违反
   C代码: 未检查 log_index <= log_len

3. 提交规则错误
   TLA+发现: 旧任期的条目不能直接提交
   C代码: 缺少 term == current_term 检查

4. 投票持久化
   TLA+发现: votedFor需要在持久化存储
   C代码: 仅在内存中保存，重启后丢失

5. 并发访问竞态
   TLA+发现: 状态机在验证中出现问题
   C代码: 缺少适当的锁保护

验证建议:
├── 在编码前编写TLA+规范
├── 将TLA+不变式映射为C断言
├── 使用模型检查结果指导测试用例
└── 定期对照TLA+规范审查代码
```

---

**文档版本**: 1.0
**创建日期**: 2026-03-19
**关联文件**: 01_TLA_Introduction.md, 03_Model_Checking.md, 05_C_Verification_with_TLA.md
**参考实现**: knowledge/04_System_Architecture/06_Distributed_Systems/ 目录下的Raft实现


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
