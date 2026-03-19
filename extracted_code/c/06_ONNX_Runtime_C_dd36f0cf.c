/*
 * Auto-generated from: 11_Machine_Learning_C\06_ONNX_Runtime_C.md
 * Line: 189
 * Language: c
 * Block ID: dd36f0cf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void print_model_info(ONNX_Runtime* rt) {
    OrtStatus* status = NULL;
    OrtAllocator* allocator = NULL;
    status = g_ort->GetAllocatorWithDefaultOptions(&allocator);
    CHECK_STATUS(status);

    // 输入数量
    size_t num_inputs;
    status = g_ort->SessionGetInputCount(rt->session, &num_inputs);
    CHECK_STATUS(status);
    printf("Number of inputs: %zu\n", num_inputs);

    // 输入信息
    for (size_t i = 0; i < num_inputs; i++) {
        char* name = NULL;
        status = g_ort->SessionGetInputName(rt->session, i, allocator, &name);
        CHECK_STATUS(status);

        OrtTypeInfo* type_info = NULL;
        status = g_ort->SessionGetInputTypeInfo(rt->session, i, &type_info);
        CHECK_STATUS(status);

        const OrtTensorTypeAndShapeInfo* tensor_info = NULL;
        status = g_ort->CastTypeInfoToTensorInfo(type_info, &tensor_info);
        CHECK_STATUS(status);

        ONNXTensorElementDataType type;
        status = g_ort->GetTensorElementType(tensor_info, &type);
        CHECK_STATUS(status);

        size_t num_dims;
        status = g_ort->GetDimensionsCount(tensor_info, &num_dims);
        CHECK_STATUS(status);

        int64_t* dims = malloc(num_dims * sizeof(int64_t));
        status = g_ort->GetDimensions(tensor_info, dims, num_dims);
        CHECK_STATUS(status);

        printf("  Input %zu: %s, type=%d, dims=[", i, name, type);
        for (size_t d = 0; d < num_dims; d++) {
            printf("%ld%s", dims[d], d < num_dims - 1 ? ", " : "");
        }
        printf("]\n");

        free(dims);
        allocator->Free(allocator, name);
        g_ort->ReleaseTypeInfo(type_info);
    }
}
