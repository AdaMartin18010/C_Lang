/-
# DistributedConsensus Aesop自动化测试

本模块测试Aesop在Raft分布式共识协议中的自动化证明能力，
包括不变式维护、状态转换验证和安全性证明。
-/

import Raft.State
import Raft.Safety
import Raft.Invariants
import Raft.AesopConfig

namespace Raft
namespace AesopTests

set_option maxHeartbeats 500000

-- ==================== 测试1: 投票唯一性 ====================

/-- 投票唯一性（自动化版本）-/
theorem voteUniqueness_aesop (s : NodeState) (term : Term) :
  ∀ candidate1 candidate2,
  s.votedFor = some candidate1 →
  s.votedFor = some candidate2 →
  candidate1 = candidate2 := by
  intros c1 c2 h1 h2
  aesop_raft

-- ==================== 测试2: 任期单调性 ====================

/-- 任期单调性（自动化版本）-/
theorem termMonotonicity_aesop {s s' : NodeState} :
  s' = s.incrementTerm → s.currentTerm < s'.currentTerm := by
  intro h
  aesop_raft

-- ==================== 测试3: 角色判定 ====================

/-- Leader角色判定（自动化）-/
theorem isLeader_def_aesop {s : NodeState} :
  isLeader s ↔ s.role = Role.Leader := by
  rfl

theorem isFollower_def_aesop {s : NodeState} :
  isFollower s ↔ s.role = Role.Follower := by
  rfl

theorem isCandidate_def_aesop {s : NodeState} :
  isCandidate s ↔ s.role = Role.Candidate := by
  rfl

-- ==================== 测试4: 已提交/已应用定义 ====================

/-- 已提交定义（自动化）-/
theorem isCommitted_def_aesop {s : NodeState} {idx : LogIndex} :
  isCommitted s idx ↔ idx ≤ s.commitIndex := by
  rfl

/-- 已应用定义（自动化）-/
theorem isApplied_def_aesop {s : NodeState} {idx : LogIndex} :
  isApplied s idx ↔ idx ≤ s.lastApplied := by
  rfl

-- ==================== 测试5: 日志操作 ====================

/-- 日志追加增加长度 -/
theorem log_append_increases_length_aesop (l : Log) (e : LogEntry) :
  l.length < (l ++ [e]).length := by
  simp
  apply Nat.lt_add_of_pos_right
  simp

-- ==================== 测试6: 节点状态转换 ====================

/-- 转换为Leader角色 -/
theorem becomeLeader_sets_role_aesop (s : NodeState) :
  (s.becomeLeader).role = Role.Leader := by
  simp [NodeState.becomeLeader]

/-- 转换为Follower角色 -/
theorem becomeFollower_sets_role_aesop (s : NodeState) (term : Term) :
  (s.becomeFollower term).role = Role.Follower := by
  simp [NodeState.becomeFollower]

/-- 转换为Candidate角色 -/
theorem becomeCandidate_sets_role_aesop (s : NodeState) (id : NodeId) :
  (s.becomeCandidate id).role = Role.Candidate := by
  simp [NodeState.becomeCandidate]

-- ==================== 测试7: 投票一致性属性 ====================

/-- 节点只能投票给有效候选者 -/
theorem canVoteFor_term_check_aesop (s : NodeState) (term : Term) (candidateId : NodeId) :
  s.canVoteFor term candidateId → s.currentTerm = term := by
  intro h
  simp [NodeState.canVoteFor] at h
  exact h.1

-- ==================== 测试8: 日志条目属性 ====================

/-- 日志条目创建时间 -/
theorem entryCreatedAt_def_aesop {entry : LogEntry} {term : Term} :
  EntryCreatedAt entry term ↔ entry.term = term := by
  rfl

/-- 条目已提交定义 -/
theorem entryCommittedIn_def_aesop {s : NodeState} {entry : LogEntry} :
  EntryCommittedIn s entry ↔ entry.index ≤ s.commitIndex := by
  rfl

-- ==================== 测试9: 任期比较 ====================

/-- 任期的传递性 -/
theorem term_le_trans_aesop {t₁ t₂ t₃ : Term} :
  t₁ ≤ t₂ → t₂ ≤ t₃ → t₁ ≤ t₃ := by
  apply Nat.le_trans

/-- 当前任期总是小于递增后的任期 -/
theorem currentTerm_lt_incremented_aesop (s : NodeState) :
  s.currentTerm < (s.incrementTerm).currentTerm := by
  simp [NodeState.incrementTerm]

-- ==================== 测试10: 复杂安全性引理（性能基准）====================

/-- Leader日志单调性（简化证明）-/
theorem leaderLogMonotonicity_aesop {s s' : NodeState} {entry : LogEntry} :
  isLeader s →
  s' = { s with log := s.log ++ [entry] } →
  s.log.length < s'.log.length := by
  intros hLeader h
  simp [h]

/-- 系统状态更新保持节点数量 -/
theorem systemState_update_preserves_nodes_aesop {n : Nat} (sys : SystemState n) 
  (id : Fin n) (st : NodeState) :
  ∀ i, (sys.updateNode id st).nodes i = if i = id then st else sys.nodes i := by
  intro i
  simp [SystemState.updateNode]

end AesopTests
end Raft
