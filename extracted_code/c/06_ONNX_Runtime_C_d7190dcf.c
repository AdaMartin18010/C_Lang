/*
 * Auto-generated from: 11_Machine_Learning_C\06_ONNX_Runtime_C.md
 * Line: 243
 * Language: c
 * Block ID: d7190dcf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

OrtValue* create_input_tensor(ONNX_Runtime* rt, float* data,
                               int64_t* dims, size_t num_dims) {
    OrtStatus* status = NULL;
    OrtValue* tensor = NULL;

    size_t total_elements = 1;
    for (size_t i = 0; i < num_dims; i++) {
        total_elements *= dims[i];
    }

    status = g_ort->CreateTensorWithDataAsOrtValue(
        rt->memory_info,
        data,                                    // 数据指针
        total_elements * sizeof(float),         // 数据大小
        dims, num_dims,                         // 维度
        ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT,    // 数据类型
        &tensor);
    CHECK_STATUS(status);

    return tensor;
}
