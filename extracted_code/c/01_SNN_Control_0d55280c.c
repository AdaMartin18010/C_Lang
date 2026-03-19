/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\01_SNN_Control.md
 * Line: 608
 * Language: c
 * Block ID: 0d55280c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 网络可能不稳定，产生过多脉冲
// 需要平衡兴奋性和抑制性连接

// ✅ 添加抑制性神经元
snn_connect_inhibitory(net, hidden_layer, 0.3f);  // 抑制权重

// 或限制最大脉冲率
if (neuron->spike_count > MAX_SPIKES_PER_STEP) {
    neuron->is_refactory = true;  // 强制不应期
}
