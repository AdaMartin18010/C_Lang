/*
 * Auto-generated from: 03_System_Technology_Domains\03_Hardware_Acceleration.md
 * Line: 433
 * Language: c
 * Block ID: b36ebcbf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// OpenCL内核代码（kernel.cl）
const char* kernel_source =
    "__kernel void vector_add(__global const float* a,\n"
    "                         __global const float* b,\n"
    "                         __global float* result,\n"
    "                         const int n) {\n"
    "    int idx = get_global_id(0);\n"
    "    if (idx < n) {\n"
    "        result[idx] = a[idx] + b[idx];\n"
    "    }\n"
    "}\n";

// 主机端代码
void vector_add_opencl(float* a, float* b, float* result, int n) {
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    cl_mem d_a, d_b, d_result;

    // 初始化OpenCL
    clGetPlatformIDs(1, &platform, NULL);
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    queue = clCreateCommandQueue(context, device, 0, NULL);

    // 创建程序
    program = clCreateProgramWithSource(context, 1, &kernel_source, NULL, NULL);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    kernel = clCreateKernel(program, "vector_add", NULL);

    // 分配内存
    size_t size = n * sizeof(float);
    d_a = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size, a, NULL);
    d_b = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, size, b, NULL);
    d_result = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size, NULL, NULL);

    // 设置参数
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_result);
    clSetKernelArg(kernel, 3, sizeof(int), &n);

    // 执行内核
    size_t global_size = ((n + 255) / 256) * 256;
    size_t local_size = 256;
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, &local_size, 0, NULL, NULL);

    // 读取结果
    clEnqueueReadBuffer(queue, d_result, CL_TRUE, 0, size, result, 0, NULL, NULL);

    // 清理
    clReleaseMemObject(d_a);
    clReleaseMemObject(d_b);
    clReleaseMemObject(d_result);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}
