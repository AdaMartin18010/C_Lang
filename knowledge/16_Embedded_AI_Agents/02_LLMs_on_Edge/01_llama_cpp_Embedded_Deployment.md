# llama.cpp 嵌入式部署指南

> **文档状态**: 🆕 新建
> **创建日期**: 2026-03-18
> **分类**: 边缘LLM / 嵌入式AI

---

## 概述

**llama.cpp**是一个纯C/C++实现的高性能LLM推理引擎，支持在CPU、GPU和嵌入式设备上运行大规模语言模型。
它是目前最流行的本地LLM部署方案之一，特别适合资源受限的边缘设备。

---


---

## 📑 目录

- [llama.cpp 嵌入式部署指南](#llamacpp-嵌入式部署指南)
  - [概述](#概述)
  - [📑 目录](#-目录)
  - [核心特性](#核心特性)
    - [技术优势](#技术优势)
    - [支持的模型](#支持的模型)
  - [硬件平台部署](#硬件平台部署)
    - [平台对比](#平台对比)
    - [在Raspberry Pi 5上部署](#在raspberry-pi-5上部署)
      - [1. 环境准备](#1-环境准备)
      - [2. 编译llama.cpp](#2-编译llamacpp)
      - [3. 下载模型](#3-下载模型)
      - [4. 运行推理](#4-运行推理)
    - [在Jetson Orin Nano上部署](#在jetson-orin-nano上部署)
  - [C/C++ API集成](#cc-api集成)
    - [基础API使用](#基础api使用)
    - [编译集成](#编译集成)
  - [服务器模式部署](#服务器模式部署)
    - [llama-server (OpenAI兼容API)](#llama-server-openai兼容api)
    - [嵌入式设备作为LLM网关](#嵌入式设备作为llm网关)
  - [性能优化](#性能优化)
    - [量化策略选择](#量化策略选择)
    - [推理加速技巧](#推理加速技巧)
  - [参考资源](#参考资源)


---

## 核心特性

### 技术优势

| 特性 | 说明 | 优势 |
|:-----|:-----|:-----|
| **纯C/C++** | 无Python依赖 | 极小部署 footprint |
| **GGUF格式** | 统一模型格式 | 跨平台兼容 |
| **量化支持** | Q4_0到Q8_0 | 4-8倍内存节省 |
| **多后端** | CPU/GPU/Metal/Vulkan | 灵活部署 |
| **流式生成** | 实时token输出 | 低延迟体验 |
| **OpenAI API** | 兼容接口 | 现有工具链复用 |

### 支持的模型

- **Meta**: LLaMA 2/3, CodeLLaMA
- **Mistral**: Mistral 7B, Mixtral 8x7B
- **Google**: Gemma 2B/7B
- **Microsoft**: Phi-2/3
- **阿里**: Qwen 系列
- **零一万物**: Yi 系列

---

## 硬件平台部署

### 平台对比

| 平台 | RAM要求 | 推荐模型 | 性能 | 功耗 |
|:-----|:--------|:---------|:-----|:-----|
| **Raspberry Pi 5** | 4-8GB | Llama-3.2-3B-Q4 | 8-12 t/s | 15W |
| **Raspberry Pi 4** | 4GB | Llama-3.2-1B-Q4 | 3-5 t/s | 8W |
| **Jetson Orin Nano** | 8GB | Llama-3.1-8B-Q4 | 15-20 t/s | 25W |
| **Intel N100** | 16GB | Llama-3.1-8B-Q8 | 20-30 t/s | 15W |
| **Apple M3** | 16GB | Llama-3.1-70B-Q4 | 20+ t/s | 20W |

### 在Raspberry Pi 5上部署

#### 1. 环境准备

```bash
# 更新系统
sudo apt update && sudo apt upgrade -y

# 安装依赖
sudo apt install -y build-essential cmake git python3-pip

# 安装Python依赖 (用于模型转换)
pip3 install huggingface-hub llama-cpp-python

# 启用大内存页 (提升性能)
echo 'vm.nr_hugepages = 128' | sudo tee -a /etc/sysctl.conf
sudo sysctl -p
```

#### 2. 编译llama.cpp

```bash
# 克隆仓库
git clone https://github.com/ggerganov/llama.cpp.git
cd llama.cpp

# 针对ARM64优化编译
make clean
make -j$(nproc) \
    GGML_NO_CUDA=1 \
    GGML_OPENBLAS=1 \
    GGML_NATIVE=1 \
    CFLAGS="-march=armv8.2-a+fp16+dotprod" \
    CXXFLAGS="-march=armv8.2-a+fp16+dotprod"

# 或使用CMake (推荐)
mkdir build && cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DGGML_OPENBLAS=ON \
    -DGGML_NATIVE=ON \
    -DCMAKE_C_FLAGS="-march=armv8.2-a+fp16+dotprod" \
    -DCMAKE_CXX_FLAGS="-march=armv8.2-a+fp16+dotprod"
make -j$(nproc)
```

#### 3. 下载模型

```bash
# 方法1: 直接下载GGUF (推荐)
mkdir -p ~/models
cd ~/models

# 下载Llama 3.2 3B Q4_K_M (约1.9GB)
wget https://huggingface.co/bartowski/Llama-3.2-3B-Instruct-GGUF/resolve/main/Llama-3.2-3B-Instruct-Q4_K_M.gguf

# 或使用方法2: 从HuggingFace转换
# pip3 install transformers
# python3 convert_hf_to_gguf.py --outfile model.gguf --outtype q4_k_m <model_path>
```

#### 4. 运行推理

```bash
# 基本推理
./build/bin/llama-cli \
    -m ~/models/Llama-3.2-3B-Instruct-Q4_K_M.gguf \
    -p "Explain quantum computing in simple terms:" \
    -n 128 \
    --temp 0.7

# 交互式聊天模式
./build/bin/llama-cli \
    -m ~/models/Llama-3.2-3B-Instruct-Q4_K_M.gguf \
    --interactive \
    --chatml \
    -n 512

# 查看性能统计
./build/bin/llama-cli \
    -m ~/models/Llama-3.2-3B-Instruct-Q4_K_M.gguf \
    -p "Hello" \
    -n 128 \
    --perf
```

### 在Jetson Orin Nano上部署

```bash
# 使用CUDA加速
mkdir build && cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DGGML_CUDA=ON \
    -DCMAKE_CUDA_ARCHITECTURES=87
make -j$(nproc)

# 运行 (自动使用GPU)
./bin/llama-cli -m model.gguf -p "Hello" -ngl 999  # -ngl 999启用所有GPU层
```

---

## C/C++ API集成

### 基础API使用

```c
// llama_integration.c
#include "llama.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    struct llama_model* model;
    struct llama_context* ctx;
    const struct llama_vocab* vocab;
} llama_wrapper_t;

// 初始化模型
llama_wrapper_t* llama_init(const char* model_path, int n_ctx) {
    llama_wrapper_t* wrapper = calloc(1, sizeof(llama_wrapper_t));

    // 模型参数
    struct llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = 0;  // CPU only for embedded

    // 加载模型
    wrapper->model = llama_load_model_from_file(model_path, model_params);
    if (!wrapper->model) {
        fprintf(stderr, "Failed to load model\n");
        free(wrapper);
        return NULL;
    }

    // 上下文参数
    struct llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = n_ctx;  // 上下文长度
    ctx_params.n_threads = 4;   // CPU线程数
    ctx_params.n_threads_batch = 4;

    // 创建上下文
    wrapper->ctx = llama_new_context_with_model(wrapper->model, ctx_params);
    if (!wrapper->ctx) {
        fprintf(stderr, "Failed to create context\n");
        llama_free_model(wrapper->model);
        free(wrapper);
        return NULL;
    }

    wrapper->vocab = llama_get_vocab(wrapper->model);

    printf("Model loaded: n_vocab=%d, n_ctx=%d\n",
           llama_vocab_n_tokens(wrapper->vocab),
           llama_n_ctx(wrapper->ctx));

    return wrapper;
}

// 生成文本
int llama_generate(llama_wrapper_t* wrapper,
                   const char* prompt,
                   char* output,
                   int max_tokens,
                   float temperature) {

    // Tokenize输入
    const int n_prompt_tokens = max_tokens + 256;
    llama_token* prompt_tokens = malloc(n_prompt_tokens * sizeof(llama_token));

    int n_tokens = llama_tokenize(
        wrapper->vocab,
        prompt,
        strlen(prompt),
        prompt_tokens,
        n_prompt_tokens,
        true,   // add special tokens
        true    // parse special
    );

    if (n_tokens < 0) {
        fprintf(stderr, "Tokenization failed\n");
        free(prompt_tokens);
        return -1;
    }

    // 创建batch
    struct llama_batch batch = llama_batch_init(n_tokens, 0, 1);

    for (int i = 0; i < n_tokens; i++) {
        llama_batch_add(batch, prompt_tokens[i], i, {0}, false);
    }
    batch.logits[batch.n_tokens - 1] = 1;  // 最后一个token需要logits

    // 解码
    if (llama_decode(wrapper->ctx, batch) != 0) {
        fprintf(stderr, "Decode failed\n");
        llama_batch_free(batch);
        free(prompt_tokens);
        return -1;
    }

    // 生成循环
    int n_cur = batch.n_tokens;
    int n_gen = 0;
    llama_token* generated = malloc(max_tokens * sizeof(llama_token));

    while (n_cur < max_tokens && n_gen < max_tokens) {
        // 采样下一个token
        llama_token new_token_id = llama_sample_token_greedy(
            wrapper->ctx,
            llama_get_logits_ith(wrapper->ctx, batch.n_tokens - 1)
        );

        // 检查结束符
        if (llama_vocab_is_eog(wrapper->vocab, new_token_id)) {
            break;
        }

        generated[n_gen++] = new_token_id;

        // 准备下一个batch
        llama_batch_clear(batch);
        llama_batch_add(batch, new_token_id, n_cur++, {0}, true);

        if (llama_decode(wrapper->ctx, batch) != 0) {
            break;
        }
    }

    // 转换token为文本
    int output_len = 0;
    for (int i = 0; i < n_gen && output_len < max_tokens - 1; i++) {
        char piece[256];
        int n = llama_token_to_piece(
            wrapper->vocab,
            generated[i],
            piece,
            sizeof(piece),
            0,  // special
            true // with space
        );
        if (n > 0 && output_len + n < max_tokens - 1) {
            memcpy(output + output_len, piece, n);
            output_len += n;
        }
    }
    output[output_len] = '\0';

    // 清理
    llama_batch_free(batch);
    free(prompt_tokens);
    free(generated);

    return n_gen;
}

// 清理资源
void llama_cleanup(llama_wrapper_t* wrapper) {
    if (wrapper) {
        llama_free(wrapper->ctx);
        llama_free_model(wrapper->model);
        free(wrapper);
    }
}

// 主函数示例
int main() {
    const char* model_path = "/home/pi/models/Llama-3.2-3B-Instruct-Q4_K_M.gguf";

    printf("Loading model...\n");
    llama_wrapper_t* llama = llama_init(model_path, 2048);
    if (!llama) {
        return 1;
    }

    const char* prompt = "User: What is TinyML?\nAssistant:";
    char response[1024];

    printf("Prompt: %s\n", prompt);
    printf("Generating...\n");

    int n_gen = llama_generate(llama, prompt, response, 512, 0.7f);

    printf("Response (%d tokens): %s\n", n_gen, response);

    llama_cleanup(llama);
    return 0;
}
```

### 编译集成

```makefile
# Makefile
LLAMA_PATH = ./llama.cpp
CC = gcc
CXX = g++
CFLAGS = -O3 -march=armv8.2-a -I$(LLAMA_PATH)/include
LDFLAGS = -L$(LLAMA_PATH)/build -lllama -lpthread -ldl

llama_integration: llama_integration.c
 $(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
 rm -f llama_integration
```

---

## 服务器模式部署

### llama-server (OpenAI兼容API)

```bash
# 启动服务器
./build/bin/llama-server \
    -m ~/models/Llama-3.2-3B-Instruct-Q4_K_M.gguf \
    --host 0.0.0.0 \
    --port 8080 \
    -n 512 \
    --timeout 30 \
    --threads-http 4

# 测试API
curl http://localhost:8080/v1/chat/completions \
  -H "Content-Type: application/json" \
  -d '{
    "model": "llama-3.2-3b",
    "messages": [{"role": "user", "content": "Hello!"}],
    "temperature": 0.7
  }'
```

### 嵌入式设备作为LLM网关

```python
# gateway.py: 在嵌入式设备上运行LLM网关
from flask import Flask, request, jsonify
import requests

app = Flask(__name__)
LLAMA_SERVER = "http://localhost:8080"

@app.route('/v1/chat/completions', methods=['POST'])
def chat_completions():
    # 转发到本地llama-server
    response = requests.post(
        f"{LLAMA_SERVER}/v1/chat/completions",
        json=request.json,
        timeout=60
    )
    return jsonify(response.json())

@app.route('/health', methods=['GET'])
def health():
    return jsonify({"status": "ok", "device": "raspberry-pi-5"})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
```

---

## 性能优化

### 量化策略选择

| 量化类型 | 精度损失 | 速度 | 大小 | 推荐场景 |
|:---------|:---------|:-----|:-----|:---------|
| **Q4_K_M** | 低 | 快 | 最小 | 大多数场景 |
| **Q5_K_M** | 很低 | 快 | 小 | 需要更高精度 |
| **Q8_0** | 极低 | 中等 | 中等 | 代码生成 |
| **FP16** | 无 | 慢 | 大 | 精度优先 |

### 推理加速技巧

```bash
# 1. 使用更大的上下文批处理
./llama-cli -m model.gguf --ctx-size 4096 --batch-size 512

# 2. 启用连续批处理 (服务器模式)
./llama-server -m model.gguf --cont-batching

# 3. 使用Flash Attention (如果编译支持)
cmake -DGGML_FLASH_ATTN=ON ..

# 4. CPU优化
# 设置线程亲和性
export OMP_PROC_BIND=spread
export OMP_PLACES=threads
```

---

## 参考资源

- [llama.cpp GitHub](https://github.com/ggerganov/llama.cpp)
- [GGUF模型格式](https://github.com/ggerganov/ggml/blob/master/docs/gguf.md)
- [HuggingFace GGUF模型](https://huggingface.co/models?search=gguf)

---

**下一章**: 02_GGML_Optimization（待创建）

**最后更新**: 2026-03-18


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
