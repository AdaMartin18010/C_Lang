/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\01_SNN_Simulation.md
 * Line: 648
 * Language: c
 * Block ID: 1f9a10fe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* snn_classifier.c - SNN模式识别示例 */
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include "snn_network.h"

/* 编码：将输入值转换为脉冲序列 */
void encode_rate_coding(const double *input, uint32_t num_inputs,
                         double *currents, double gain)
{
    for (uint32_t i = 0; i < num_inputs; i++) {
        /* 输入值映射到注入电流 */
        currents[i] = input[i] * gain;
    }
}

/* 解码：基于发放率解码 */
uint32_t decode_rate_winner_take_all(const double *rates, uint32_t num_outputs)
{
    uint32_t max_idx = 0;
    double max_rate = rates[0];

    for (uint32_t i = 1; i < num_outputs; i++) {
        if (rates[i] > max_rate) {
            max_rate = rates[i];
            max_idx = i;
        }
    }

    return max_idx;
}

/* 训练SNN分类器 */
void train_snn_classifier(SNNNetwork *net,
                           const double **training_data,
                           const uint32_t *labels,
                           uint32_t num_samples,
                           uint32_t num_epochs)
{
    uint32_t input_layer = 0;
    uint32_t output_layer = net->num_layers - 1;
    Layer *in_layer = &net->layers[input_layer];

    double *input_currents = calloc(in_layer->num_neurons, sizeof(double));
    bool *output_spikes = calloc(net->layers[output_layer].num_neurons, sizeof(bool));

    net->enable_stdp = true;

    printf("开始训练...\n");

    for (uint32_t epoch = 0; epoch < num_epochs; epoch++) {
        uint32_t correct = 0;

        for (uint32_t sample = 0; sample < num_samples; sample++) {
            /* 编码输入 */
            encode_rate_coding(training_data[sample], in_layer->num_neurons,
                               input_currents, 20.0);

            /* 重置网络 */
            for (uint32_t i = 0; i < net->num_neurons; i++) {
                lif_neuron_init(&net->neurons[i],
                                &net->layers[0].neuron_params);
            }
            net->current_time = 0.0;

            /* 运行仿真500ms */
            for (double t = 0; t < 500.0; t += net->dt) {
                snn_apply_input(net, input_layer, input_currents);
                snn_update(net);
            }

            /* 检查输出层对应类别的脉冲 */
            snn_get_output_spikes(net, output_layer, output_spikes);

            /* 奖励调制STDP (简化版本) */
            uint32_t predicted = 0;
            uint32_t max_spikes = 0;
            for (uint32_t i = 0; i < net->layers[output_layer].num_neurons; i++) {
                uint32_t spike_count = net->neurons[net->layers[output_layer].start_idx + i].spike_count;
                if (spike_count > max_spikes) {
                    max_spikes = spike_count;
                    predicted = i;
                }
            }

            if (predicted == labels[sample]) {
                correct++;
            }
        }

        printf("Epoch %u: 准确率 = %.2f%%\n",
               epoch + 1, (double)correct / num_samples * 100);
    }

    net->enable_stdp = false;
    free(input_currents);
    free(output_spikes);
}

/* 测试SNN分类器 */
void test_snn_classifier(SNNNetwork *net,
                          const double **test_data,
                          const uint32_t *labels,
                          uint32_t num_samples)
{
    uint32_t input_layer = 0;
    uint32_t output_layer = net->num_layers - 1;
    Layer *in_layer = &net->layers[input_layer];

    double *input_currents = calloc(in_layer->num_neurons, sizeof(double));
    double *output_rates = calloc(net->layers[output_layer].num_neurons, sizeof(double));

    uint32_t correct = 0;

    printf("\n测试结果:\n");

    for (uint32_t sample = 0; sample < num_samples; sample++) {
        /* 编码输入 */
        encode_rate_coding(test_data[sample], in_layer->num_neurons,
                           input_currents, 20.0);

        /* 重置网络 */
        for (uint32_t i = 0; i < net->num_neurons; i++) {
            lif_neuron_init(&net->neurons[i], &net->layers[0].neuron_params);
        }
        net->current_time = 0.0;

        /* 运行仿真 */
        for (double t = 0; t < 500.0; t += net->dt) {
            snn_apply_input(net, input_layer, input_currents);
            snn_update(net);
        }

        /* 解码输出 */
        snn_get_output_rates(net, output_layer, output_rates, 500.0);
        uint32_t predicted = decode_rate_winner_take_all(output_rates,
                                                          net->layers[output_layer].num_neurons);

        printf("样本 %u: 预测=%u, 真实=%u", sample, predicted, labels[sample]);
        if (predicted == labels[sample]) {
            printf(" [正确]\n");
            correct++;
        } else {
            printf(" [错误]\n");
        }
    }

    printf("\n总准确率: %.2f%% (%u/%u)\n",
           (double)correct / num_samples * 100, correct, num_samples);

    free(input_currents);
    free(output_rates);
}

/* 主函数 */
int main(int argc, char *argv[])
{
    (void)argc; (void)argv;

    printf("=== SNN脉冲神经网络分类器示例 ===\n\n");

    /* 创建网络 */
    SNNNetwork net;
    snn_init(&net, 1.0);  /* 1ms时间步长 */

    /* 定义神经元参数 */
    LIFParams lif_params;
    lif_init_params(&lif_params, 20.0, -70.0, -70.0, -55.0, 10.0, 2.0);

    /* 添加层：输入(4) - 隐藏(10) - 输出(3) */
    int input_layer = snn_add_layer(&net, LAYER_INPUT, 4, &lif_params);
    int hidden_layer = snn_add_layer(&net, LAYER_HIDDEN, 10, &lif_params);
    int output_layer = snn_add_layer(&net, LAYER_OUTPUT, 3, &lif_params);

    printf("网络结构:\n");
    printf("  输入层: 4个神经元\n");
    printf("  隐藏层: 10个神经元\n");
    printf("  输出层: 3个神经元\n\n");

    /* 连接层 */
    snn_connect_layers(&net, (uint32_t)input_layer, (uint32_t)hidden_layer,
                       1.0, 0.5, 0.1);
    snn_connect_layers(&net, (uint32_t)hidden_layer, (uint32_t)output_layer,
                       1.0, 0.5, 0.1);

    printf("突触连接: %u\n\n", net.num_synapses);

    /* 示例数据 (简化版Iris数据集) */
    const double training_data[][4] = {
        {5.1, 3.5, 1.4, 0.2},  /* 类别0 */
        {4.9, 3.0, 1.4, 0.2},
        {4.7, 3.2, 1.3, 0.2},
        {7.0, 3.2, 4.7, 1.4},  /* 类别1 */
        {6.4, 3.2, 4.5, 1.5},
        {6.9, 3.1, 4.9, 1.5},
        {6.3, 3.3, 6.0, 2.5},  /* 类别2 */
        {5.8, 2.7, 5.1, 1.9},
        {7.1, 3.0, 5.9, 2.1},
    };

    const uint32_t labels[] = {0, 0, 0, 1, 1, 1, 2, 2, 2};
    const uint32_t num_samples = 9;

    /* 训练 */
    const double *data_ptrs[9];
    for (uint32_t i = 0; i < num_samples; i++) {
        data_ptrs[i] = training_data[i];
    }

    train_snn_classifier(&net, data_ptrs, labels, num_samples, 10);

    /* 测试 */
    test_snn_classifier(&net, data_ptrs, labels, num_samples);

    return 0;
}
