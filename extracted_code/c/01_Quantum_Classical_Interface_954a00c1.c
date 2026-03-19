/*
 * Auto-generated from: 04_Industrial_Scenarios\06_Quantum_Computing\01_Quantum_Classical_Interface.md
 * Line: 823
 * Language: c
 * Block ID: 954a00c1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 问题: 相邻门时间重叠
// Gate1: t=0, duration=25ns
// Gate2: t=20, duration=25ns (与Gate1重叠5ns)

// ✅ 正确: 检查时间不重叠
bool check_pulse_overlap(const PulseSequence *seq,
                         uint32_t start_ns, uint32_t duration_ns) {
    uint32_t end_ns = start_ns + duration_ns;

    for (uint32_t i = 0; i < seq->num_pulses; i++) {
        uint32_t p_start = seq->pulses[i].start_time_ns;
        uint32_t p_end = p_start + seq->pulses[i].duration_ns;

        // 检查是否有重叠
        if (!(end_ns <= p_start || start_ns >= p_end)) {
            return true;  // 有重叠
        }
    }
    return false;
}
