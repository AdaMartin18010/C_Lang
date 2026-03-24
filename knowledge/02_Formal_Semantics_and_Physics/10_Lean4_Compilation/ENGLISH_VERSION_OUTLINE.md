# English Version Outline - Programming Language Semantics

> **Version**: 1.0
> **Scope**: Core semantics documents English structure
> **Target**: International audience

---

## Overview

This document outlines the structure for translating the comprehensive semantics knowledge base into English. The goal is to make this resource accessible to the global research and education community.

### Translation Priority

| Priority | Document | Rationale |
|:---------|:---------|:----------|
| P0 | SEMANTIC_NOTATIONS.md | Foundation for all |
| P0 | SEMANTICS_UNIFIED_REVISED.md | Core theory |
| P1 | BRIDGE_THEOREMS.md | Key contribution |
| P1 | LEAN4_SEMANTICS_MAPPING.md | Practical value |
| P2 | Applied Semantics (3 docs) | Domain coverage |
| P2 | Advanced Theory (2 docs) | Research depth |
| P3 | Case Studies (3 docs) | Practical examples |
| P3 | Frontier Research (3 docs) | Cutting-edge |
| P4 | History & Education | Supplementary |

---

## Core Documents Structure

### 1. SEMANTIC_NOTATIONS.md → SEMANTIC_NOTATIONS_EN.md

**Sections**:

1. Introduction
2. Operational Semantics Notations
   - Single-step reduction: M → M'
   - Multi-step reduction: M →* N
   - Big-step evaluation: M ⇓ v
3. Denotational Semantics Notations
   - Meaning function: ⟦M⟧
   - Environment: ρ
   - Bottom element: ⊥
4. Axiomatic Semantics Notations
   - Hoare triple: {P} C {Q}
   - Weakest precondition: wp(C, Q)
5. Type Theory Notations
6. Category Theory Notations
7. Deprecated Notations
8. Migration Guide

### 2. SEMANTICS_UNIFIED_REVISED.md → SEMANTICS_UNIFIED_EN.md

**Sections**:

1. Introduction
2. The Three Semantic Paradigms
   2.1 Operational Semantics
   2.2 Denotational Semantics
   2.3 Axiomatic Semantics
3. Core Connection Theorems (with proofs)
   3.1 Operational-Denotational Equivalence
   3.2 Type Safety Theorem
   3.3 Hoare Logic Completeness
4. Logical Relations
5. Correspondence Table
6. Lean 4 Connection
7. Bridge Theorems Overview
8. Edge Cases
9. Advanced Topics Overview
10. References

### 3. BRIDGE_THEOREMS.md → BRIDGE_THEOREMS_EN.md

**Sections**:

1. Introduction
2. BT-1: Operational to Denotational
3. BT-2: Denotational to Operational
4. BT-3: Denotational to Axiomatic
5. BT-4: Axiomatic to Denotational
6. BT-5: Operational-Axiomatic Direct
7. Composition of Bridges
8. Applications
9. References

---

## Applied Semantics Documents

### 4. APPLIED_FUNCTIONAL.md → APPLIED_FUNCTIONAL_EN.md

**Sections**:

1. Introduction
2. Lambda Calculus Variants
   2.1 Untyped Lambda Calculus
   2.2 Simply Typed Lambda Calculus
   2.3 System F (Polymorphic)
   2.4 Recursive Types
3. ML Type System
   3.1 Let-polymorphism
   3.2 Value Restriction
   3.3 Polymorphic References
   3.4 Type Inference Algorithm W
4. Haskell Lazy Semantics
5. Comparison with Lean 4
6. Formal Theorems and Proofs
7. References

### 5. APPLIED_IMPERATIVE.md → APPLIED_IMPERATIVE_EN.md

**Sections**:

1. Introduction
2. While Language Semantics
3. Hoare Logic
4. C Language Subset
5. Separation Logic
6. Formal Theorems and Proofs
7. References

### 6. APPLIED_CONCURRENT.md → APPLIED_CONCURRENT_EN.md

**Sections**:

1. Introduction
2. Labeled Transition Systems
3. CCS (Calculus of Communicating Systems)
4. CSP (Communicating Sequential Processes)
5. π-Calculus
6. Bisimulation Theory
7. Formal Theorems and Proofs
8. References

---

## Advanced Theory Documents

### 7. CATEGORICAL_SEMANTICS.md → CATEGORICAL_SEMANTICS_EN.md

**Sections**:

1. Introduction
2. Category Theory Foundations
3. CCC and Lambda Calculus
4. Adjunctions
5. Monad Semantics
6. Coalgebras and Infinite Structures
7. Formal Theorems and Proofs
8. References

### 8. ALGEBRAIC_EFFECTS.md → ALGEBRAIC_EFFECTS_EN.md

**Sections**:

1. Introduction
2. Effect Theory Foundations
3. Effect Handlers
4. Concrete Effect Examples
5. Algebraic Effects vs Monads
6. Advanced Topics
7. Formal Theorems and Proofs
8. References

---

## Case Study Documents

### 9. CASE_STUDY_LEAN_KERNEL.md → CASE_STUDY_LEAN_KERNEL_EN.md

**Sections**:

1. Introduction
2. Lean 4 Kernel Architecture
3. Type System
4. Reduction Mechanisms
5. Metatheory
6. Theory Correspondences
7. Formal Theorems and Proofs
8. References

### 10. CASE_STUDY_TYPECHECKER.md → CASE_STUDY_TYPECHECKER_EN.md

**Sections**:

1. Introduction
2. Target Language
3. Type Checker Implementation
4. Soundness Verification
5. Completeness Verification
6. Extensions
7. Formal Theorems and Proofs
8. References

### 11. CASE_STUDY_UNIFICATION.md → CASE_STUDY_UNIFICATION_EN.md

**Sections**:

1. Introduction
2. First-order Unification
3. Algorithm Implementation
4. Higher-order Unification
5. Type Inference Applications
6. Lean 4 Unification
7. Formal Theorems and Proofs
8. References

---

## Frontier Research Documents

### 12. FRONTIER_PROBABILISTIC.md → FRONTIER_PROBABILISTIC_EN.md

**Sections**:

1. Introduction
2. Probabilistic Programming Foundations
3. Measure-theoretic Semantics
4. Probabilistic Lambda Calculus
5. Inference Algorithms
6. Semantic Correctness
7. Formal Theorems and Proofs
8. References

### 13. FRONTIER_DIFFERENTIAL.md → FRONTIER_DIFFERENTIAL_EN.md

**Sections**:

1. Introduction
2. Privacy Foundations
3. Semantic Definitions
4. Type Systems
5. Verification Techniques
6. Advanced Topics
7. Formal Theorems and Proofs
8. References

### 14. FRONTIER_QUANTUM.md → FRONTIER_QUANTUM_EN.md

**Sections**:

1. Introduction
2. Quantum Mechanics Foundations
3. Quantum Lambda Calculus
4. Linear Type Systems
5. Quantum Circuits
6. Advanced Topics
7. Formal Theorems and Proofs
8. References

---

## Supporting Documents

### 15. HISTORY_SEMANTICS.md → HISTORY_SEMANTICS_EN.md

**Sections**:

1. Introduction
2. Prehistory (1930-1960)
3. Origins of Operational Semantics (1960-1975)
4. Birth of Denotational Semantics (1964-1980)
5. Development of Axiomatic Semantics (1967-1980)
6. Type Theory Revolution (1970-1990)
7. Categorical Semantics (1970-1990)
8. Concurrency Theory (1980-2000)
9. Modern Developments (2000-2024)
10. Key Figures
11. Milestone Timeline
12. References

### 16. EXERCISES_SOLUTIONS.md → EXERCISES_SOLUTIONS_EN.md

**Structure**:

- 50+ exercises with solutions
- 4 difficulty levels
- Coverage: Operational, Denotational, Axiomatic, Types, Categories, Concurrency
- Open problems section

### 17. IMPLEMENTATION_GUIDE.md → IMPLEMENTATION_GUIDE_EN.md

**Sections**:

1. Introduction
2. Project Setup
3. Lexical Analysis
4. Syntax Analysis
5. Type Checker
6. Operational Semantics Implementation
7. Verification and Testing
8. Extensions
9. Best Practices
10. Complete Example (MiniML)
11. References

---

## Translation Guidelines

### Terminology Consistency

| Chinese | English | Notes |
|:--------|:--------|:------|
| 操作语义 | Operational Semantics | |
| 指称语义 | Denotational Semantics | |
| 公理语义 | Axiomatic Semantics | |
| 类型保持 | Type Preservation | |
| 进展性 | Progress | |
| 合流性 | Confluence | |
| 最强后置条件 | Strongest Postcondition | |
| 最弱前置条件 | Weakest Precondition | |
| 同余 | Congruence | |
| 互模拟 | Bisimulation | |

### Formatting Standards

1. **Code Blocks**: Keep Lean 4 code unchanged
2. **Mathematical Notation**: Use LaTeX, keep consistent
3. **Citations**: Translate titles, keep original references
4. **Examples**: Adapt cultural references if necessary
5. **Diagrams**: Recreate with English labels

### Quality Assurance

- Native speaker review
- Technical accuracy check
- Consistency verification
- Link validation

---

## Community Contribution

### How to Contribute

1. Fork the repository
2. Choose a document to translate
3. Follow the outline structure
4. Submit PR for review

### Review Process

1. Technical review (accuracy)
2. Language review (fluency)
3. Consistency check
4. Integration approval

---

## Timeline

| Phase | Duration | Deliverables |
|:------|:---------|:-------------|
| Phase 1 | 2 months | P0 documents (3 docs) |
| Phase 2 | 3 months | P1 documents (2 docs) |
| Phase 3 | 4 months | P2 documents (5 docs) |
| Phase 4 | 3 months | P3 documents (6 docs) |
| Phase 5 | 2 months | P4 documents (4 docs) |
| **Total** | **14 months** | **20 core documents** |

---

## Resources

### Translation Tools

- DeepL for initial draft
- Grammarly for polishing
- LaTeX for mathematics
- Git for version control

### Reference Materials

- Pierce's TAPL (Types and Programming Languages)
- Winskel's Formal Semantics
- Harper's Practical Foundations for Programming Languages
- Verified Functional Algorithms in Lean

---

## Contact

For questions about the English translation:

- Email: <semantics-i18n@lean4-lang.org>
- GitHub Issues: github.com/lean4-lang/semantics/issues
- Discussions: github.com/lean4-lang/semantics/discussions

---

**Maintainer**: Lean 4 Semantics Internationalization Team
**Last Updated**: 2026-03-24
