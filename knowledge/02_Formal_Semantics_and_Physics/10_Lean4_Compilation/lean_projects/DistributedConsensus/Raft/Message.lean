/-
# Raft消息传递协议

本模块定义Raft协议的消息类型、RPC语义和消息处理逻辑。

## 消息类型
1. RequestVote RPC：选举请求
2. AppendEntries RPC：日志复制和心跳
3. ClientRequest/Reply：客户端通信

## 安全属性
- 消息必须包含正确的任期号
- 任期更新是单调的
- 消息处理触发相应的状态转换
-/

import Std
import Raft.State

namespace Raft

-- ==================== RPC消息定义 ====================

/-- RequestVote RPC请求

候选者发起选举时发送，包含候选者的日志状态信息，
以便Follower判断是否应该投票。
-/
structure RequestVoteArgs where
  term : Term                    -- 候选者的任期
  candidateId : NodeId           -- 候选者ID
  lastLogIndex : LogIndex        -- 候选者最后日志索引
  lastLogTerm : Term             -- 候选者最后日志任期
  deriving Repr, BEq

/-- RequestVote RPC回复 -/
structure RequestVoteReply where
  term : Term                    -- 当前任期（用于候选者更新自己）
  voteGranted : Bool             -- 是否投票给候选者
  voterId : NodeId               -- 回复者ID
  deriving Repr, BEq

/-- AppendEntries RPC请求

Leader用于：
1. 复制日志条目到Followers
2. 发送心跳（空entries列表）
-/
structure AppendEntriesArgs where
  term : Term                    -- Leader的任期
  leaderId : NodeId              -- Leader的ID（用于Follower重定向）
  prevLogIndex : LogIndex        -- 新条目之前的日志索引
  prevLogTerm : Term             -- prevLogIndex处条目的任期
  entries : List LogEntry        -- 要存储的日志条目（心跳时为空）
  leaderCommit : LogIndex        -- Leader的commitIndex
  deriving Repr, BEq

/-- AppendEntries RPC回复 -/
structure AppendEntriesReply where
  term : Term                    -- 当前任期（用于Leader更新自己）
  success : Bool                 -- 如果包含匹配prevLogIndex和prevLogTerm的条目则为true
  followerId : NodeId            -- 回复者ID
  matchIndex : LogIndex          -- Follower的匹配索引（用于Leader优化）
  deriving Repr, BEq

-- ==================== 客户端通信 ====================

/-- 客户端请求 -/
structure ClientRequestArgs where
  clientId : ClientId            -- 客户端标识
  seqNum : SequenceNum           -- 序列号（去重）
  command : Command              -- 要执行的命令
  deriving Repr, BEq

/-- 客户端回复 -/
structure ClientReply where
  success : Bool                 -- 请求是否成功
  result : String                -- 执行结果或错误信息
  leaderId : Option NodeId       -- 如果不是Leader，指示Leader位置
  deriving Repr, BEq

-- ==================== 消息处理 ====================

namespace RequestVoteArgs

/-- 创建选举请求 -/
def mkRequest (s : NodeState) (candidateId : NodeId) : RequestVoteArgs where
  term := s.currentTerm
  candidateId := candidateId
  lastLogIndex := s.log.lastIndex
  lastLogTerm := s.log.lastTerm

/-- 检查候选者的日志是否至少和接收者一样新 -/
def logUpToDate (args : RequestVoteArgs) (receiverLog : Log) : Bool :=
  let lastTerm := receiverLog.lastTerm
  if args.lastLogTerm ≠ lastTerm then
    args.lastLogTerm ≥ lastTerm
  else
    args.lastLogIndex ≥ receiverLog.lastIndex

end RequestVoteArgs

namespace RequestVoteReply

/-- 创建投票回复 -/
def mkReply (s : NodeState) (voteGranted : Bool) (voterId : NodeId) : RequestVoteReply where
  term := s.currentTerm
  voteGranted := voteGranted
  voterId := voterId

end RequestVoteReply

namespace AppendEntriesArgs

/-- 创建心跳消息 -/
def mkHeartbeat (s : NodeState) (leaderId : NodeId) : AppendEntriesArgs where
  term := s.currentTerm
  leaderId := leaderId
  prevLogIndex := s.log.lastIndex
  prevLogTerm := s.log.lastTerm
  entries := []
  leaderCommit := s.commitIndex

/-- 创建日志复制请求 -/
def mkAppend (s : NodeState) (leaderId : NodeId) 
             (nextIdx : LogIndex) (entries : List LogEntry) : AppendEntriesArgs where
  term := s.currentTerm
  leaderId := leaderId
  prevLogIndex := nextIdx - 1
  prevLogTerm := s.log.termAt (nextIdx - 1)
  entries := entries
  leaderCommit := s.commitIndex

end AppendEntriesArgs

namespace AppendEntriesReply

/-- 创建AppendEntries回复 -/
def mkReply (s : NodeState) (success : Bool) (followerId : NodeId) : AppendEntriesReply where
  term := s.currentTerm
  success := success
  followerId := followerId
  matchIndex := if success then s.log.lastIndex else 0

end AppendEntriesReply

-- ==================== 消息处理逻辑 ====================

/-- 处理RequestVote请求，返回(新状态, 回复) -/
def handleRequestVote (s : NodeState) (args : RequestVoteArgs) 
                      (selfId : NodeId) : NodeState × RequestVoteReply :=
  -- 规则1：如果term < currentTerm，拒绝投票
  if args.term < s.currentTerm then
    let reply := RequestVoteReply.mkReply s false selfId
    (s, reply)
  else
    -- 更新任期（如果需要）
    let s' := if args.term > s.currentTerm 
              then s.becomeFollower args.term 
              else s
    
    -- 检查是否可以投票
    let canVote := s'.canVoteFor args.term args.candidateId
    let logOk := args.logUpToDate s'.log
    
    if canVote ∧ logOk then
      let s'' := { s' with votedFor := some args.candidateId }
      let reply := RequestVoteReply.mkReply s'' true selfId
      (s'', reply)
    else
      let reply := RequestVoteReply.mkReply s' false selfId
      (s', reply)

/-- 处理AppendEntries请求，返回(新状态, 回复) -/
def handleAppendEntries (s : NodeState) (args : AppendEntriesArgs) 
                        (selfId : NodeId) : NodeState × AppendEntriesReply :=
  -- 规则1：如果term < currentTerm，拒绝
  if args.term < s.currentTerm then
    let reply := AppendEntriesReply.mkReply s false selfId
    (s, reply)
  else
    -- 更新任期并成为Follower（如果需要）
    let s' := if args.term > s.currentTerm 
              then s.becomeFollower args.term
              else s.becomeFollower s.currentTerm
    
    -- 重置选举超时
    let s' := { s' with lastHeartbeat := 0 }
    
    -- 规则2：如果日志在prevLogIndex处没有匹配的条目，拒绝
    if args.prevLogIndex > 0 then
      match s'.log.getEntry? args.prevLogIndex with
      | none => 
        let reply := AppendEntriesReply.mkReply s' false selfId
        (s', reply)
      | some entry =>
        if entry.term ≠ args.prevLogTerm then
          -- 规则3：删除现有条目和所有后续条目
          let s'' := { s' with log := s'.log.truncateTo (args.prevLogIndex - 1) }
          let reply := AppendEntriesReply.mkReply s'' false selfId
          (s'', reply)
        else
          -- 追加新条目
          let newLog := s'.log ++ args.entries
          let s'' := { s' with log := newLog }
          -- 更新commitIndex
          let s''' := if args.leaderCommit > s''.commitIndex then
                        { s'' with commitIndex := min args.leaderCommit newLog.length }
                      else s''
          let reply := AppendEntriesReply.mkReply s''' true selfId
          (s''', reply)
    else
      -- prevLogIndex = 0，直接追加
      let newLog := s'.log ++ args.entries
      let s'' := { s' with log := newLog }
      let s''' := if args.leaderCommit > s''.commitIndex then
                    { s'' with commitIndex := min args.leaderCommit newLog.length }
                  else s''
      let reply := AppendEntriesReply.mkReply s''' true selfId
      (s''', reply)

/-- 处理RequestVote回复 -/
def handleRequestVoteReply (s : NodeState) (reply : RequestVoteReply) 
                           (votes : Nat) (config : ClusterConfig) : NodeState × Nat × Bool :=
  -- 更新任期（如果需要）
  let s' := if reply.term > s.currentTerm 
            then s.becomeFollower reply.term 
            else s
  
  -- 计算票数
  let newVotes := if reply.voteGranted then votes + 1 else votes
  
  -- 检查是否赢得选举
  let won := newVotes ≥ config.quorumSize
  
  -- 如果赢得选举，成为Leader
  let s'' := if won ∧ s'.role = Role.Candidate then s'.becomeLeader else s'
  
  (s'', newVotes, won)

/-- 处理AppendEntries回复 -/
def handleAppendEntriesReply (s : NodeState) (reply : AppendEntriesReply) 
                             (followerId : NodeId) : NodeState :=
  -- 更新任期（如果需要）
  let s' := if reply.term > s.currentTerm 
            then s.becomeFollower reply.term 
            else s
  
  -- 如果成功，更新matchIndex和nextIndex
  if reply.success then
    let s'' := { s' with 
      matchIndex := fun i => if i = followerId then reply.matchIndex else s'.matchIndex i,
      nextIndex := fun i => if i = followerId then reply.matchIndex + 1 else s'.nextIndex i }
    s''
  else
    -- 如果失败，递减nextIndex重试
    let s'' := { s' with 
      nextIndex := fun i => if i = followerId 
                            then max 1 (s'.nextIndex i - 1) 
                            else s'.nextIndex i }
    s''

-- ==================== 领导者操作 ====================

/-- Leader广播心跳 -/
def broadcastHeartbeat (s : NodeState) (leaderId : NodeId) 
                       (config : ClusterConfig) : List (NodeId × AppendEntriesArgs) :=
  if s.role ≠ Role.Leader then []
  else
    config.nodes.filter (· ≠ leaderId) |>.map (fun nodeId =>
      let args := AppendEntriesArgs.mkHeartbeat s leaderId
      (nodeId, args))

/-- Leader发送日志条目给特定节点 -/
def sendEntriesTo (s : NodeState) (leaderId : NodeId) (followerId : NodeId) 
                  : Option (NodeId × AppendEntriesArgs) :=
  if s.role ≠ Role.Leader then none
  else
    let nextIdx := s.nextIndex followerId
    let entries := s.log.filter (fun e => e.index ≥ nextIdx)
    if entries.isEmpty then none
    else
      let args := AppendEntriesArgs.mkAppend s leaderId nextIdx entries
      some (followerId, args)

/-- Leader尝试提交日志条目 -/
def tryCommit (s : NodeState) (config : ClusterConfig) : NodeState :=
  if s.role ≠ Role.Leader then s
  else
    -- 找到可以提交的N：N > commitIndex且多数节点matchIndex ≥ N
    let potentialN := s.log.filter (fun e => 
      e.index > s.commitIndex ∧ 
      e.term = s.currentTerm ∧  -- Leader只能提交当前任期的条目
      (config.nodes.filter (fun n => s.matchIndex n ≥ e.index)).length ≥ config.quorumSize
    ) |>.map (·.index) |>.maximum?
    
    match potentialN with
    | some n => { s with commitIndex := n }
    | none => s

end Raft
