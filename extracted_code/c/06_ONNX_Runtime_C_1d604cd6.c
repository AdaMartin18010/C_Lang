/*
 * Auto-generated from: 11_Machine_Learning_C\06_ONNX_Runtime_C.md
 * Line: 269
 * Language: c
 * Block ID: 1d604cd6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    float* data;
    int64_t* shape;
    size_t num_dims;
    size_t total_size;
} Tensor_Output;

Tensor_Output* onnx_inference(ONNX_Runtime* rt, OrtValue** inputs,
                               size_t num_inputs) {
    OrtStatus* status = NULL;

    // 输出数组
    OrtValue* outputs[1] = {NULL};

    // 运行会话
    const char* input_names[] = {"input"};  // 根据模型修改
    const char* output_names[] = {"output"};

    status = g_ort->Run(rt->session, NULL,
                        input_names, inputs, num_inputs,
                        output_names, 1,
                        outputs);
    CHECK_STATUS(status);

    // 获取输出数据
    float* output_data;
    status = g_ort->GetTensorMutableData(outputs[0], (void**)&output_data);
    CHECK_STATUS(status);

    // 获取输出形状
    OrtTensorTypeAndShapeInfo* info = NULL;
    status = g_ort->GetTensorTypeAndShape(outputs[0], &info);
    CHECK_STATUS(status);

    size_t num_dims;
    g_ort->GetDimensionsCount(info, &num_dims);
    int64_t* shape = malloc(num_dims * sizeof(int64_t));
    g_ort->GetDimensions(info, shape, num_dims);

    size_t total_size;
    g_ort->GetTensorShapeElementCount(info, &total_size);

    // 复制数据（因为outputs会被释放）
    float* data_copy = malloc(total_size * sizeof(float));
    memcpy(data_copy, output_data, total_size * sizeof(float));

    Tensor_Output* result = malloc(sizeof(Tensor_Output));
    result->data = data_copy;
    result->shape = shape;
    result->num_dims = num_dims;
    result->total_size = total_size;

    g_ort->ReleaseTensorTypeAndShapeInfo(info);
    g_ort->ReleaseValue(outputs[0]);

    return result;
}

void free_tensor_output(Tensor_Output* tensor) {
    if (tensor) {
        free(tensor->data);
        free(tensor->shape);
        free(tensor);
    }
}
