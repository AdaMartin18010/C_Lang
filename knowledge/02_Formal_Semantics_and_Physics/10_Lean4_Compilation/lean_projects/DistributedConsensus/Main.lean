/-
# Raft共识算法形式化 - 主程序

本程序演示Raft共识算法的Lean 4形式化实现，包括：
1. 状态机转换演示
2. 选举过程模拟
3. 日志复制示例
4. 安全性定理验证
5. Paxos协议对比

## 运行方式
```bash
lake build
lake exe raft-consensus
```
-/

import Raft.State
import Raft.Message
import Raft.Safety
import Raft.Liveness
import Raft.Invariants
import Raft.Paxos

namespace DistributedConsensus

open Raft

-- ==================== 演示辅助函数 ====================

def printHeader (title : String) : IO Unit := do
  IO.println ""
  IO.println s!"╔══════════════════════════════════════════════════════════════╗"
  IO.println s!"║  {title.leftpad 58 ' '}".take 63 ++ "║"
  IO.println s!"╚══════════════════════════════════════════════════════════════╝"

def printSection (title : String) : IO Unit := do
  IO.println ""
  IO.println s!"--- {title} ---"

def printState (s : NodeState) (id : NodeId) : IO Unit := do
  IO.println s!"  Node {id}: {s.role} | Term: {s.currentTerm} | Log: {s.log.length} entries | Commit: {s.commitIndex}"

-- ==================== 演示1：节点状态机 ====================

def demoNodeStateMachine : IO Unit := do
  printSection "节点状态机转换"
  
  let node0 := NodeState.init 0
  IO.println "初始状态:"
  printState node0 0
  
  -- 成为Candidate
  let node1 := node0.becomeCandidate 0
  IO.println "\n成为Candidate（开始选举）:"
  printState node1 0
  
  -- 成为Leader
  let node2 := node1.becomeLeader
  IO.println "\n成为Leader（赢得选举）:"
  printState node2 0
  
  -- 追加日志条目
  let (node3, entry) := node2.appendEntry "set x = 42" 1 1
  IO.println s!"\n追加日志条目: {entry}"
  printState node3 0
  
  -- 成为Follower（发现更高任期）
  let node4 := node3.becomeFollower 2
  IO.println "\n成为Follower（发现更高任期）:"
  printState node4 0

-- ==================== 演示2：日志操作 ====================

def demoLogOperations : IO Unit := do
  printSection "日志操作演示"
  
  let mut log : Log := []
  
  -- 添加条目
  for i in [1:6] do
    let entry := LogEntry.mk 1 i s!"command_{i}" 1 i
    log := log ++ [entry]
    IO.println s!"追加: [{entry.term}:{entry.index}] {entry.command}"
  
  IO.println s!"\n日志长度: {log.length}"
  IO.println s!"最后索引: {log.lastIndex}"
  IO.println s!"最后任期: {log.lastTerm}"
  
  -- 查询条目
  match log.getEntry? 3 with
  | some e => IO.println s!"\n索引3的条目: [{e.term}:{e.index}] {e.command}"
  | none => IO.println "\n索引3无条目"
  
  -- 截断日志
  let truncated := log.truncateTo 3
  IO.println s!"\n截断到索引3后长度: {truncated.length}"

-- ==================== 演示3：选举RPC ====================

def demoElectionRPC : IO Unit := do
  printSection "选举RPC演示"
  
  let candidate := (NodeState.init 1).becomeCandidate 1
  IO.println "Candidate状态:"
  printState candidate 1
  
  -- 创建选举请求
  let request := RequestVoteArgs.mkRequest candidate 1
  IO.println s!"\n选举请求:"
  IO.println s!"  Term: {request.term}"
  IO.println s!"  CandidateId: {request.candidateId}"
  IO.println s!"  LastLogIndex: {request.lastLogIndex}"
  IO.println s!"  LastLogTerm: {request.lastLogTerm}"
  
  -- Follower处理请求
  let follower := NodeState.init 0
  IO.println "\nFollower状态:"
  printState follower 0
  
  let (newFollower, reply) := handleRequestVote follower request 0
  IO.println s!"\n处理结果:"
  IO.println s!"  Follower新任期: {newFollower.currentTerm}"
  IO.println s!"  投票结果: {if reply.voteGranted then "同意" else "拒绝"}"

-- ==================== 演示4：日志复制RPC ====================

def demoLogReplication : IO Unit := do
  printSection "日志复制演示"
  
  -- Leader状态
  let mut leader := (NodeState.init 0).becomeLeader
  let (leader, entry1) := leader.appendEntry "set x = 10" 1 1
  let (leader, entry2) := leader.appendEntry "set y = 20" 1 2
  
  IO.println "Leader状态（有2条日志）:"
  printState leader 0
  
  -- 创建AppendEntries请求
  let args := AppendEntriesArgs.mkAppend leader 0 1 [entry1, entry2]
  IO.println s!"\nAppendEntries请求:"
  IO.println s!"  Term: {args.term}"
  IO.println s!"  PrevLogIndex: {args.prevLogIndex}"
  IO.println s!"  PrevLogTerm: {args.prevLogTerm}"
  IO.println s!"  Entries: {args.entries.length} 条"
  IO.println s!"  LeaderCommit: {args.leaderCommit}"
  
  -- Follower处理
  let follower := NodeState.init 1
  let (newFollower, reply) := handleAppendEntries follower args 1
  IO.println s!"\nFollower处理结果:"
  printState newFollower 1
  IO.println s!"  成功: {reply.success}"

-- ==================== 演示5：安全性不变式 ====================

def demoSafetyInvariants : IO Unit := do
  printSection "安全性不变式说明"
  
  IO.println "关键不变式1: 选举安全性"
  IO.println "  每个任期最多只有一个Leader"
  IO.println "  证明基础：多数投票原理 + 每节点每任期一票"
  
  IO.println "\n关键不变式2: 领导者日志完整性"
  IO.println "  已提交条目存在于所有未来Leader的日志中"
  IO.println "  证明基础：新Leader必须包含多数节点的日志"
  
  IO.println "\n关键不变式3: 状态机安全"
  IO.println "  已应用到状态机的命令不会被覆盖"
  IO.println "  证明基础：已提交条目不再被修改"
  
  IO.println "\n关键不变式4: 任期单调性"
  IO.println "  节点任期只增不减"
  IO.println "  证明基础：节点只接受更高任期的消息"

-- ==================== 演示6：Paxos对比 ====================

def demoPaxosComparison : IO Unit := do
  printSection "Raft vs Paxos 对比"
  
  IO.println "Raft特点:"
  IO.println "  ✓ 强Leader模型，逻辑清晰"
  IO.println "  ✓ 日志复制和Leader选举统一"
  IO.println "  ✓ 工程实现友好"
  IO.println "  ✗ 需要Leader在线才能处理写请求"
  
  IO.println "\nPaxos特点:"
  IO.println "  ✓ 无Leader也可以处理请求"
  IO.println "  ✓ 数学基础扎实"
  IO.println "  ✗ 理解和实现复杂"
  IO.println "  ✗ 多Proposer竞争时性能差"
  
  IO.println "\n消息复杂度对比:"
  IO.println "  Raft（稳定期）: 1轮广播（AppendEntries）"
  IO.println "  Paxos: 2轮（Prepare/Promise + Accept/Accepted）"
  IO.println "  Multi-Paxos（稳定期）: 1轮（类似Raft）"

-- ==================== 演示7：工业案例分析 ====================

def demoIndustrialCases : IO Unit := do
  printSection "工业案例分析"
  
  IO.println "etcd/Raft-RS:"
  IO.println "  - Go语言实现，云原生基础设施"
  IO.println "  - 支持线性一致性读"
  IO.println "  - 成员变更支持Joint Consensus"
  IO.println "  - 预投票机制防止网络分区干扰"
  
  IO.println "\nTiKV/Raft-RS:"
  IO.println "  - Rust实现，高性能分布式KV"
  IO.println "  - 基于RocksDB的持久化"
  IO.println "  - Batch和Pipeline优化"
  
  IO.println "\nConsul:"
  IO.println "  - Serf gossip + Raft"
  IO.println "  - 服务发现+KV存储"
  IO.println "  - 多数据中心支持"

-- ==================== 主函数 ====================

def main : IO Unit := do
  printHeader "Raft Consensus Algorithm Formalization in Lean 4"
  
  IO.println ""
  IO.println "本演示展示Raft共识算法的Lean 4形式化实现："
  IO.println "  • 状态机定义（Leader/Follower/Candidate）"
  IO.println "  • 日志复制机制"
  IO.println "  • 选举和投票逻辑"
  IO.println "  • 安全性定理"
  IO.println "  • 与Paxos的对比"
  
  demoNodeStateMachine
  demoLogOperations
  demoElectionRPC
  demoLogReplication
  demoSafetyInvariants
  demoPaxosComparison
  demoIndustrialCases
  
  printHeader "Demo Complete"
  IO.println ""
  IO.println "完整代码位于: knowledge/02_Formal_Semantics_and_Physics/"
  IO.println "             10_Lean4_Compilation/lean_projects/DistributedConsensus/"
  IO.println ""
  IO.println "要构建和运行："
  IO.println "  cd lean_projects/DistributedConsensus"
  IO.println "  lake build"
  IO.println "  lake exe raft-consensus"

end DistributedConsensus
