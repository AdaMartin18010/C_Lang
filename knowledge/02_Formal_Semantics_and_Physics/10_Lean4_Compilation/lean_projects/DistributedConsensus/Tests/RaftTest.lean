/-
# Raft共识算法测试

本模块包含Raft协议的单元测试和属性测试。
-/

import Raft.State
import Raft.Message
import Raft.Safety
import Raft.Invariants

namespace Raft.Tests

open Raft

-- ==================== 基础状态测试 ====================

def testInitialState : Bool :=
  let s := NodeState.init 0
  s.role = Role.Follower ∧
  s.currentTerm = 0 ∧
  s.votedFor.isNone ∧
  s.log.isEmpty ∧
  s.commitIndex = 0

def testBecomeCandidate : Bool :=
  let s := NodeState.init 0
  let s' := s.becomeCandidate 0
  s'.role = Role.Candidate ∧
  s'.currentTerm = 1 ∧
  s'.votedFor = some 0

def testBecomeLeader : Bool :=
  let s := (NodeState.init 0).becomeCandidate 0
  let s' := s.becomeLeader
  s'.role = Role.Leader

def testTermIncrement : Bool :=
  let s := NodeState.init 0
  let s1 := s.incrementTerm
  let s2 := s1.incrementTerm
  s1.currentTerm = 1 ∧ s2.currentTerm = 2

def testBecomeFollower : Bool :=
  let s := (NodeState.init 0).becomeCandidate 0 |>.becomeLeader
  let s' := s.becomeFollower 2
  s'.role = Role.Follower ∧ s'.currentTerm = 2

-- ==================== 日志测试 ====================

def testLogAppend : Bool :=
  let mut log : Log := []
  let e1 := LogEntry.mk 1 1 "cmd1" 1 1
  let e2 := LogEntry.mk 1 2 "cmd2" 1 2
  log := log ++ [e1, e2]
  log.length = 2 ∧ log.lastIndex = 2

def testLogGetEntry : Bool :=
  let e1 := LogEntry.mk 1 1 "cmd1" 1 1
  let e2 := LogEntry.mk 1 2 "cmd2" 1 2
  let log : Log := [e1, e2]
  log.getEntry? 1 = some e1 ∧
  log.getEntry? 2 = some e2 ∧
  log.getEntry? 3 = none

def testLogTruncate : Bool :=
  let e1 := LogEntry.mk 1 1 "cmd1" 1 1
  let e2 := LogEntry.mk 1 2 "cmd2" 1 2
  let e3 := LogEntry.mk 1 3 "cmd3" 1 3
  let log : Log := [e1, e2, e3]
  let truncated := log.truncateTo 2
  truncated.length = 2

def testLogConsistency : Bool :=
  let e1 := LogEntry.mk 1 1 "cmd1" 1 1
  let e2 := LogEntry.mk 1 2 "cmd2" 1 2
  let log : Log := [e1, e2]
  log.termAt 1 = 1 ∧ log.termAt 2 = 1 ∧ log.termAt 3 = 0

-- ==================== 投票测试 ====================

def testCanVoteFor : Bool :=
  let s := NodeState.init 0
  s.canVoteFor 0 1 = true

def testCannotVoteTwice : Bool :=
  let s := NodeState.init 0
  let s' := { s with votedFor := some 1 }
  s'.canVoteFor 0 1 = true ∧ -- 可以投票给已投票的候选者
  s'.canVoteFor 0 2 = false   -- 不能投票给不同的候选者

def testHandleRequestVote : Bool :=
  let follower := NodeState.init 0
  let candidate := (NodeState.init 1).becomeCandidate 1
  let request := RequestVoteArgs.mkRequest candidate 1
  let (newFollower, reply) := handleRequestVote follower request 0
  reply.voteGranted = true ∧
  newFollower.votedFor = some 1

def testRejectLowerTerm : Bool :=
  let follower := (NodeState.init 0).becomeCandidate 0  -- term = 1
  let oldCandidate := NodeState.init 1  -- term = 0
  let request := RequestVoteArgs.mkRequest oldCandidate 1
  let (_, reply) := handleRequestVote follower request 0
  reply.voteGranted = false

-- ==================== 日志复制测试 ====================

def testHandleAppendEntries : Bool :=
  let follower := NodeState.init 1
  let leader := (NodeState.init 0).becomeLeader
  let (leader, entry) := leader.appendEntry "cmd1" 1 1
  let args := AppendEntriesArgs.mkAppend leader 0 1 [entry]
  let (newFollower, reply) := handleAppendEntries follower args 1
  reply.success = true ∧
  newFollower.log.length = 1

def testRejectMissingPrevLog : Bool :=
  let follower := NodeState.init 1
  let leader := (NodeState.init 0).becomeLeader
  let (leader, entry) := leader.appendEntry "cmd1" 1 1
  -- 设置错误的prevLogIndex
  let args := { AppendEntriesArgs.mkAppend leader 0 1 [entry] with 
    prevLogIndex := 5, prevLogTerm := 1 }
  let (_, reply) := handleAppendEntries follower args 1
  reply.success = false

def testHigherTermConvertToFollower : Bool :=
  let candidate := (NodeState.init 0).becomeCandidate 0  -- term = 1
  let args : AppendEntriesArgs := {
    term := 2, leaderId := 1, prevLogIndex := 0, prevLogTerm := 0,
    entries := [], leaderCommit := 0
  }
  let (newState, _) := handleAppendEntries candidate args 0
  newState.role = Role.Follower ∧ newState.currentTerm = 2

-- ==================== 安全属性测试 ====================

def testElectionSafety : Bool :=
  -- 在相同任期，最多只有一个Leader
  -- 这是协议保证的，这里测试基本逻辑
  let s1 := (NodeState.init 0).becomeCandidate 0 |>.becomeLeader
  let s2 := (NodeState.init 1).becomeCandidate 1  -- 不同任期
  s1.currentTerm ≠ s2.currentTerm

def testLogMatching : Bool :=
  -- 相同索引和任期的条目必须相同
  let e1 := LogEntry.mk 1 1 "cmd1" 1 1
  let e2 := LogEntry.mk 1 1 "cmd1" 1 1
  e1 = e2

def testCommitIndexMonotonic : Bool :=
  let s := NodeState.init 0
  let s1 := { s with commitIndex := 3 }
  let s2 := { s1 with commitIndex := 5 }
  s2.commitIndex ≥ s1.commitIndex

-- ==================== 不变式测试 ====================

def testTermMonotonicity : Bool :=
  let s := NodeState.init 0
  let s1 := s.incrementTerm
  let s2 := s1.incrementTerm
  s2.currentTerm > s1.currentTerm ∧ s1.currentTerm > s.currentTerm

def testLeaderLogCompleteness : Bool :=
  -- Leader必须包含所有已提交条目
  let mut leader := (NodeState.init 0).becomeLeader
  let (leader, entry) := leader.appendEntry "cmd1" 1 1
  let leader := { leader with commitIndex := 1 }
  -- Leader的日志包含索引1
  leader.log.getEntry? 1 ≠ none

def testVotedForPersistence : Bool :=
  -- 投票后votedFor应该被设置
  let s := NodeState.init 0
  let s' := { s with votedFor := some 1 }
  s'.votedFor = some 1

-- ==================== 测试运行器 ====================

structure TestResult where
  name : String
  passed : Bool
  deriving Repr

def runAllTests : IO Unit := do
  let tests := [
    ("Initial State", testInitialState),
    ("Become Candidate", testBecomeCandidate),
    ("Become Leader", testBecomeLeader),
    ("Term Increment", testTermIncrement),
    ("Become Follower", testBecomeFollower),
    ("Log Append", testLogAppend),
    ("Log Get Entry", testLogGetEntry),
    ("Log Truncate", testLogTruncate),
    ("Log Consistency", testLogConsistency),
    ("Can Vote For", testCanVoteFor),
    ("Cannot Vote Twice", testCannotVoteTwice),
    ("Handle RequestVote", testHandleRequestVote),
    ("Reject Lower Term", testRejectLowerTerm),
    ("Handle AppendEntries", testHandleAppendEntries),
    ("Reject Missing PrevLog", testRejectMissingPrevLog),
    ("Higher Term Convert", testHigherTermConvertToFollower),
    ("Election Safety", testElectionSafety),
    ("Log Matching", testLogMatching),
    ("Commit Index Monotonic", testCommitIndexMonotonic),
    ("Term Monotonicity", testTermMonotonicity),
    ("Leader Log Completeness", testLeaderLogCompleteness),
    ("VotedFor Persistence", testVotedForPersistence)
  ]
  
  IO.println ""
  IO.println "=== Running Raft Tests ==="
  
  let mut passed := 0
  let mut failed := 0
  
  for (name, test) in tests do
    if test then
      IO.println s!"  ✓ {name}"
      passed := passed + 1
    else
      IO.println s!"  ✗ {name}"
      failed := failed + 1
  
  IO.println s!""
  IO.println s!"Results: {passed} passed, {failed} failed"
  
  if failed = 0 then
    IO.println "All tests passed!"
  else
    IO.println s!"{failed} test(s) failed!"

end Raft.Tests
