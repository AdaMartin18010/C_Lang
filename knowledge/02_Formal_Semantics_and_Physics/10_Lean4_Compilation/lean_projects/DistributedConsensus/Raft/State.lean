/-
# Raft状态机定义

本模块定义Raft共识算法的核心状态机，包括：
- 节点角色（Leader/Follower/Candidate）
- 节点状态（任期、日志、投票记录等）
- 状态转换规则

## 参考
- Ongaro & Ousterhout, "In Search of an Understandable Consensus Algorithm", 2014
- Raft PhD Dissertation, Stanford University
- etcd/Raft-RS实现
- TLA+ Raft Specification
- Verdi Coq Implementation

## 设计原则
1. 状态机清晰分离
2. 不变式内建于类型系统
3. 事件驱动转换
-/

import Std

namespace Raft

-- ==================== 基础定义 ====================

/-- 节点ID，使用自然数表示 -/
abbrev NodeId := Nat

/-- 任期号(Term)，单调递增的自然数 -/
abbrev Term := Nat

/-- 日志索引，从1开始 -/
abbrev LogIndex := Nat

/-- 日志条目中的命令/操作 -/
abbrev Command := String

/-- 客户端请求ID -/
abbrev ClientId := Nat

/-- 请求序列号 -/
abbrev SequenceNum := Nat

-- ==================== 节点角色 ====================

/-- 节点在Raft协议中的角色 -/
inductive Role where
  | Follower   -- 跟随者：接收并响应来自Leader的请求
  | Candidate  -- 候选者：发起选举以成为Leader
  | Leader     -- 领导者：处理客户端请求，复制日志
  deriving Repr, BEq, DecidableEq, Inhabited

namespace Role

/-- 角色转换为可读字符串 -/
def toString : Role → String
  | Follower => "Follower"
  | Candidate => "Candidate"
  | Leader => "Leader"

instance : ToString Role where
  toString := toString

end Role

-- ==================== 日志条目 ====================

/-- 单个日志条目

每个日志条目包含：
- 任期号：该条目被创建时的任期
- 索引：在日志中的位置
- 命令：客户端请求的操作
- 客户端ID和序列号：用于去重和线性一致性
-/
structure LogEntry where
  term : Term                    -- 条目创建的任期
  index : LogIndex               -- 日志索引
  command : Command              -- 客户端命令
  clientId : ClientId            -- 发起请求的客户端
  seqNum : SequenceNum           -- 客户端序列号（去重用）
  deriving Repr, BEq, DecidableEq

namespace LogEntry

/-- 创建新的日志条目 -/
def mk (term : Term) (index : LogIndex) (cmd : Command) 
       (cid : ClientId) (seq : SequenceNum) : LogEntry :=
  { term := term, index := index, command := cmd,
    clientId := cid, seqNum := seq }

/-- 比较两个日志条目是否相同（仅比较内容和位置） -/
def contentEq (e1 e2 : LogEntry) : Bool :=
  e1.term = e2.term ∧ e1.index = e2.index ∧ e1.command = e2.command

instance : ToString LogEntry where
  toString e := s!"[{e.term}:{e.index}] {e.command}"

end LogEntry

-- ==================== 日志 ====================

/-- 日志是日志条目的序列

Raft的日志是一个前缀有序的序列，保证：
1. 如果两个日志条目有相同的索引和任期，则它们存储相同的命令
2. 如果两个日志条目有相同的索引和任期，则它们之前的所有条目都相同
-/
abbrev Log := List LogEntry

namespace Log

/-- 空日志 -/
def empty : Log := []

/-- 获取日志长度 -/
def length (l : Log) : Nat := l.length

/-- 获取最后一个日志条目的索引，空日志返回0 -/
def lastIndex (l : Log) : LogIndex :=
  match l with
  | [] => 0
  | e :: _ => e.index

/-- 获取指定索引的条目（索引从1开始） -/
def getEntry? (l : Log) (idx : LogIndex) : Option LogEntry :=
  l.find? (fun e => e.index = idx)

/-- 获取指定索引条目的任期，不存在返回0 -/
def termAt (l : Log) (idx : LogIndex) : Term :=
  match getEntry? l idx with
  | some e => e.term
  | none => 0

/-- 在日志末尾追加新条目 -/
def append (l : Log) (e : LogEntry) : Log := l ++ [e]

/-- 截断日志到指定索引（包含），保留前缀 -/
def truncateTo (l : Log) (idx : LogIndex) : Log :=
  l.filter (fun e => e.index ≤ idx)

/-- 获取日志中最后一个条目的任期，空日志返回0 -/
def lastTerm (l : Log) : Term :=
  match l with
  | [] => 0
  | e :: _ => e.term

/-- 检查日志是否包含指定索引和任期的条目 -/
def contains (l : Log) (idx : LogIndex) (term : Term) : Bool :=
  match getEntry? l idx with
  | some e => e.term = term
  | none => false

/-- 检查一个日志是否比另一个更新

更新定义为：
- 比较最后一个条目的任期
- 如果任期相同，比较长度
-/
def isMoreUpToDate (l1 l2 : Log) : Bool :=
  let t1 := l1.lastTerm
  let t2 := l2.lastTerm
  if t1 ≠ t2 then t1 > t2 else l1.length ≥ l2.length

/-- 检查两个日志在指定索引之前是否一致 -/
def consistentUpTo (l1 l2 : Log) (idx : LogIndex) : Prop :=
  ∀ i, i ≤ idx → termAt l1 i = termAt l2 i

/-- 日志前缀匹配性质：如果两个日志在某索引处有相同的条目，
    则它们在该索引之前的所有条目都相同（归纳性质） -/
theorem prefixMatchProperty : ∀ (l1 l2 : Log) (idx : LogIndex),
  getEntry? l1 idx = getEntry? l2 idx →
  (∀ i < idx, termAt l1 i = termAt l2 i) := by
  sorry -- 需要归纳证明

end Log

-- ==================== 节点状态 ====================

/-- 单个节点的完整状态

包含Raft协议所需的所有持久和易失状态。
-/
structure NodeState where
  -- 持久状态（需要存储到磁盘）
  currentTerm : Term             -- 当前任期
  votedFor : Option NodeId       -- 当前任期投票给谁（null表示未投票）
  log : Log                      -- 日志条目

  -- 易失状态（重启后重置）
  role : Role                    -- 当前角色
  commitIndex : LogIndex         -- 已知的最高已提交条目索引
  lastApplied : LogIndex         -- 状态机应用的最高条目索引

  -- Leader特有状态（重启后重置）
  nextIndex : NodeId → LogIndex  -- 每个Follower的下一条日志索引
  matchIndex : NodeId → LogIndex -- 每个Follower已知复制的最高索引
  
  -- 时间相关状态
  lastHeartbeat : Nat            -- 上次收到心跳的时间戳
  electionTimeout : Nat          -- 选举超时时间

  deriving Inhabited

namespace NodeState

/-- 创建新的节点状态 -/
def init (id : NodeId) : NodeState where
  currentTerm := 0
  votedFor := none
  log := Log.empty
  role := Role.Follower
  commitIndex := 0
  lastApplied := 0
  nextIndex := fun _ => 1
  matchIndex := fun _ => 0
  lastHeartbeat := 0
  electionTimeout := 150 + id * 10  -- 分散超时避免活锁

/-- 判断是否可以在指定任期为某节点投票 -/
def canVoteFor (s : NodeState) (term : Term) (candidateId : NodeId) : Bool :=
  s.currentTerm = term ∧ 
  (s.votedFor.isNone ∨ s.votedFor = some candidateId)

/-- 递增当前任期 -/
def incrementTerm (s : NodeState) : NodeState :=
  { s with currentTerm := s.currentTerm + 1, votedFor := none }

/-- 转换为Candidate角色并开始新选举 -/
def becomeCandidate (s : NodeState) (selfId : NodeId) : NodeState :=
  let s' := s.incrementTerm
  { s' with 
    role := Role.Candidate,
    votedFor := some selfId }

/-- 转换为Leader角色 -/
def becomeLeader (s : NodeState) : NodeState :=
  { s with 
    role := Role.Leader,
    nextIndex := fun _ => s.log.length + 1,
    matchIndex := fun _ => 0 }

/-- 转换为Follower角色，可能更新任期 -/
def becomeFollower (s : NodeState) (term : Term) : NodeState :=
  let s' := if term > s.currentTerm 
            then { s with currentTerm := term, votedFor := none }
            else s
  { s' with role := Role.Follower }

/-- 追加日志条目 -/
def appendEntry (s : NodeState) (cmd : Command) (cid : ClientId) (seq : SequenceNum) 
                : NodeState × LogEntry :=
  let newIndex := s.log.length + 1
  let entry := LogEntry.mk s.currentTerm newIndex cmd cid seq
  ({ s with log := s.log.append entry }, entry)

/-- 更新提交索引 -/
def setCommitIndex (s : NodeState) (idx : LogIndex) : NodeState :=
  { s with commitIndex := idx }

/-- 检查是否可以提交指定索引的条目 -/
def canCommit (s : NodeState) (idx : LogIndex) : Bool :=
  idx > s.commitIndex ∧ 
  s.log.termAt idx = s.currentTerm ∧  -- Leader只能提交当前任期的条目
  idx ≤ s.log.length

end NodeState

-- ==================== 集群配置 ====================

/-- 集群配置 -/
structure ClusterConfig where
  nodes : List NodeId           -- 所有节点ID列表
  quorumSize : Nat              -- 法定多数大小（通常 floor(n/2) + 1）
  
/-- 创建集群配置 -/
def ClusterConfig.mk (nodes : List NodeId) : ClusterConfig where
  nodes := nodes
  quorumSize := nodes.length / 2 + 1

/-- 检查节点是否在集群中 -/
def ClusterConfig.contains (c : ClusterConfig) (id : NodeId) : Bool :=
  id ∈ c.nodes

-- ==================== 系统状态 ====================

/-- 整个分布式系统的全局状态 -/
structure SystemState (n : Nat) where
  nodes : Fin n → NodeState     -- n个节点的状态
  config : ClusterConfig        -- 集群配置
  network : List Message        -- 网络中的消息（简化模型）

/-- 消息类型（在Message.lean中详细定义） -/
inductive Message where
  | RequestVote (term : Term) (candidateId : NodeId) 
                (lastLogIndex : LogIndex) (lastLogTerm : Term)
  | RequestVoteReply (term : Term) (voteGranted : Bool)
  | AppendEntries (term : Term) (leaderId : NodeId)
                  (prevLogIndex : LogIndex) (prevLogTerm : Term)
                  (entries : List LogEntry) (leaderCommit : LogIndex)
  | AppendEntriesReply (term : Term) (success : Bool) (matchIndex : LogIndex)
  | ClientRequest (clientId : ClientId) (seqNum : SequenceNum) (command : Command)
  | ClientReply (success : Bool) (result : String)
  deriving Repr, BEq

namespace SystemState

/-- 初始化系统状态 -/
def init {n : Nat} (config : ClusterConfig) : SystemState n :=
  { nodes := fun i => NodeState.init i.val,
    config := config,
    network := [] }

/-- 获取指定节点的状态 -/
def getNode {n : Nat} (s : SystemState n) (id : Fin n) : NodeState :=
  s.nodes id

/-- 更新指定节点的状态 -/
def updateNode {n : Nat} (s : SystemState n) (id : Fin n) (st : NodeState) : SystemState n :=
  { s with nodes := fun i => if i = id then st else s.nodes i }

end SystemState

-- ==================== 事件和动作 ====================

/-- 系统事件类型 -/
inductive Event where
  | ElectionTimeout (node : NodeId)      -- 选举超时
  | HeartbeatTimeout (node : NodeId)     -- 心跳超时
  | MessageReceive (node : NodeId) (msg : Message)  -- 收到消息
  | ClientRequest (client : ClientId) (cmd : Command)  -- 客户端请求
  deriving Repr

/-- 节点执行的动作 -/
inductive Action where
  | SendMessage (to : NodeId) (msg : Message)
  | CommitEntries (indices : List LogIndex)
  | ApplyToStateMachine (entry : LogEntry)
  | StartElection
  | BroadcastHeartbeat
  deriving Repr

end Raft
