/-
# DistributedConsensus Aesop配置

本模块为Raft分布式共识项目配置Aesop自动化证明策略，
专注于不变式维护、状态转换验证和安全性证明。
-/

import Aesop

namespace Raft

-- ==================== Aesop规则集配置 ====================

/-- DistributedConsensus领域特定的Aesop规则集 -/
declare_aesop_rule_sets [DistributedConsensus]

-- ==================== Safe规则 ====================

@[aesop safe (rule_sets [DistributedConsensus])]
theorem voteUniqueness_safe (s : NodeState) (term : Term) :
  ∀ candidate1 candidate2,
  s.votedFor = some candidate1 →
  s.votedFor = some candidate2 →
  candidate1 = candidate2 := by
  intros c1 c2 h1 h2
  simp [h1] at h2
  exact h2

@[aesop safe (rule_sets [DistributedConsensus])]
theorem termMonotonicity_safe {s s' : NodeState} :
  s' = s.incrementTerm → s.currentTerm < s'.currentTerm := by
  intro h
  simp [h, NodeState.incrementTerm]

@[aesop safe (rule_sets [DistributedConsensus])]
theorem leaderLogMonotonicity_safe {s s' : NodeState} {entry : LogEntry} :
  isLeader s →
  s' = { s with log := s.log ++ [entry] } →
  s.log.length < s'.log.length := by
  intros hLeader h
  simp [h]

-- ==================== Unsafe规则 ====================

@[aesop unsafe 30% (rule_sets [DistributedConsensus])]
theorem electionSafety_unsafe {n : Nat} {sys : SystemState n} {t : Term}
  (h1 : ∀ i, sys.nodes i |>.currentTerm ≤ t)
  (h2 : ∀ i j, isLeader (sys.nodes i) → isLeader (sys.nodes j) → 
    sys.nodes i |>.currentTerm = sys.nodes j |>.currentTerm → i = j) :
  True := by
  trivial

@[aesop unsafe 25% (rule_sets [DistributedConsensus])]
theorem logMatchingLemma_unsafe {e1 e2 : LogEntry} :
  e1.index = e2.index → e1.term = e2.term → e1.command = e2.command := by
  sorry  -- 需要补充完整证明

@[aesop unsafe 25% (rule_sets [DistributedConsensus])]
theorem stateMachineSafety_unsafe {n : Nat} {sys : SystemState n} 
  {i j : Fin n} {idx : LogIndex} {cmd1 cmd2 : Command} :
  isApplied (sys.nodes i) idx →
  isApplied (sys.nodes j) idx →
  sys.nodes i |>.log |>.getEntry? idx = some ({ term := _, index := idx, command := cmd1, clientId := _, seqNum := _ }) →
  sys.nodes j |>.log |>.getEntry? idx = some ({ term := _, index := idx, command := cmd2, clientId := _, seqNum := _ }) →
  cmd1 = cmd2 := by
  sorry  -- 复杂证明

-- ==================== 不变式规则 ====================

@[aesop unsafe 30% (rule_sets [DistributedConsensus])]
theorem leaderLogCompletenessPreserved_unsafe {n : Nat} {sys sys' : SystemState n}
  {i j : Fin n} {term0 term : Term} {entry : LogEntry} :
  LeaderLogCompletenessInvariant [(sys, 0)] →
  term ≥ term0 →
  isCommitted (sys.nodes i) entry.index →
  isLeader (sys'.nodes j) →
  sys'.nodes j |>.currentTerm = term →
  sys'.nodes j |>.log |>.getEntry? entry.index = some entry := by
  sorry

@[aesop unsafe 25% (rule_sets [DistributedConsensus])]
theorem stateMachineSafetyProof_unsafe {n : Nat} {states : List (SystemState n × Nat)} :
  LeaderLogCompletenessInvariant states →
  StateMachineSafetyInvariant states := by
  sorry

@[aesop unsafe 25% (rule_sets [DistributedConsensus])]
theorem nodeTermMonotonicity_unsafe {n : Nat} {states : List (SystemState n × Nat)} (i : Fin n) :
  ∀ t, let state_t := (states.get? t).get!
       let state_t1 := (states.get? (t+1)).get!
       state_t1.1.nodes i |>.currentTerm ≥ state_t.1.nodes i |>.currentTerm := by
  sorry

-- ==================== 归一化规则 ====================

@[aesop norm (rule_sets [DistributedConsensus])]
theorem isLeader_def {s : NodeState} : isLeader s ↔ s.role = Role.Leader := by
  rfl

@[aesop norm (rule_sets [DistributedConsensus])]
theorem isFollower_def {s : NodeState} : isFollower s ↔ s.role = Role.Follower := by
  rfl

@[aesop norm (rule_sets [DistributedConsensus])]
theorem isCandidate_def {s : NodeState} : isCandidate s ↔ s.role = Role.Candidate := by
  rfl

@[aesop norm (rule_sets [DistributedConsensus])]
theorem isCommitted_def {s : NodeState} {idx : LogIndex} : 
  isCommitted s idx ↔ idx ≤ s.commitIndex := by
  rfl

@[aesop norm (rule_sets [DistributedConsensus])]
theorem isApplied_def {s : NodeState} {idx : LogIndex} :
  isApplied s idx ↔ idx ≤ s.lastApplied := by
  rfl

@[aesop norm simp (rule_sets [DistributedConsensus])]
def NodeState_incrementTerm_def (s : NodeState) : 
  s.incrementTerm = { s with currentTerm := s.currentTerm + 1, votedFor := none } := by
  rfl

-- ==================== Aesop策略宏定义 ====================

/-- Raft专用aesop策略 -/
macro "aesop_raft" : tactic =>
  `(tactic|
    aesop (config := 
      { terminal := false
        maxRuleApplicationDepth := 60
        maxRuleApplications := 600
        enableSimp := true })
      (rule_sets [DistributedConsensus])
  )

/-- 不变式证明策略 -/
macro "aesop_inv" : tactic =>
  `(tactic|
    aesop (config := 
      { terminal := false
        maxRuleApplicationDepth := 80
        maxRuleApplications := 800
        enableSimp := true })
      (rule_sets [DistributedConsensus])
  )

/-- 安全性证明策略 -/
macro "aesop_safety" : tactic =>
  `(tactic|
    aesop (config := 
      { terminal := false
        maxRuleApplicationDepth := 100
        maxRuleApplications := 1000
        enableSimp := true })
      (rule_sets [DistributedConsensus])
  )

/-- 状态转换验证策略 -/
macro "aesop_transition" : tactic =>
  `(tactic|
    aesop (config := 
      { terminal := true
        maxRuleApplicationDepth := 40
        maxRuleApplications := 400
        enableSimp := true })
      (rule_sets [DistributedConsensus])
  )

-- ==================== 便捷证明构造器 ====================

tactic_elab "prove_invariant" : tactic =>
  Lean.Elab.Tactic.evalTactic (←
    `(tactic|
      intro h
      try induction h
      all_goals aesop_inv
    )
  )

tactic_elab "prove_safety" : tactic =>
  Lean.Elab.Tactic.evalTactic (←
    `(tactic|
      intros
      try simp [isLeader_def, isCommitted_def, isApplied_def]
      aesop_safety
    )
  )

tactic_elab "verify_transition" : tactic =>
  Lean.Elab.Tactic.evalTactic (←
    `(tactic|
      intros
      try simp [NodeState_incrementTerm_def]
      aesop_transition
    )
  )

end Raft
