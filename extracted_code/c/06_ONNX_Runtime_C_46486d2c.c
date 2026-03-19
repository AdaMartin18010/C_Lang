/*
 * Auto-generated from: 11_Machine_Learning_C\06_ONNX_Runtime_C.md
 * Line: 407
 * Language: c
 * Block ID: 46486d2c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 启用TensorRT (NVIDIA GPU)
#ifdef USE_TENSORRT
OrtTensorRTProviderOptionsV2* tensorrt_options = NULL;
g_ort->CreateTensorRTProviderOptions(&tensorrt_options);
g_ort->SessionOptionsAppendExecutionProvider_TensorRT_V2(
    rt->session_options, tensorrt_options);
g_ort->ReleaseTensorRTProviderOptions(tensorrt_options);
#endif

// 启用OpenVINO (Intel)
#ifdef USE_OPENVINO
g_ort->SessionOptionsAppendExecutionProvider_OpenVINO(
    rt->session_options, "");
#endif

// 启用CoreML (Apple)
#ifdef USE_COREML
 OrtCoreMLProviderOptions coreml_options = {0};
 coreml_options.use_cpu_only = 0;
 coreml_options.enable_on_subgraph = 1;
 g_ort->SessionOptionsAppendExecutionProvider_CoreML(
     rt->session_options, &coreml_options);
#endif
