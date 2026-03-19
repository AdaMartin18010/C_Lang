/*
 * Auto-generated from: 11_Machine_Learning_C\06_ONNX_Runtime_C.md
 * Line: 477
 * Language: c
 * Block ID: dc51a3e6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用固定内存池避免动态分配
#define MAX_TENSOR_SIZE (1024 * 1024)  // 1MB
static float g_tensor_pool[MAX_TENSOR_SIZE];

OrtValue* create_tensor_from_pool(ONNX_Runtime* rt, int64_t* dims,
                                   size_t num_dims) {
    size_t total = 1;
    for (size_t i = 0; i < num_dims; i++) total *= dims[i];

    if (total > MAX_TENSOR_SIZE) {
        fprintf(stderr, "Tensor too large for pool\n");
        return NULL;
    }

    OrtValue* tensor = NULL;
    g_ort->CreateTensorWithDataAsOrtValue(
        rt->memory_info,
        g_tensor_pool,
        total * sizeof(float),
        dims, num_dims,
        ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT,
        &tensor);

    return tensor;
}
