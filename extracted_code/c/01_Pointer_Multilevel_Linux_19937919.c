/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Multilevel_Linux.md
 * Line: 436
 * Language: c
 * Block ID: 19937919
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Linux内核模块参数处理: kernel/params.c
// 处理字符串数组参数，使用三级指针

// 模块参数结构
struct kernel_param {
    const char *name;
    u16 flags;
    u16 type;
    union {
        void *arg;           // 一级指针
        const struct kernel_param_ops *ops;
    };
};

// 字符串数组参数处理（三级指针场景）
// char **array → array[i]指向一个字符串

typedef int (*set_fn)(const char *val,
                      const struct kernel_param *kp);
typedef int (*get_fn)(char *buffer,
                      const struct kernel_param *kp);

// 设置字符串数组参数
// val: 传入的参数值（字符串）
// kp->arg: 指向 char** (二级指针)
int param_set_charp(const char *val,
                    const struct kernel_param *kp) {
    // kp->arg 是 char** 类型（二级指针）
    char **arg = (char **)kp->arg;

    // 修改二级指针指向的内容（即修改一级指针的值）
    *arg = kstrdup(val, GFP_KERNEL);
    if (!*arg)
        return -ENOMEM;

    return 0;
}

// 使用示例
static char *my_string = NULL;  // 一级指针

// 模块参数定义
static const struct kernel_param my_param = {
    .name = "my_string",
    .arg = &my_string,  // 二级指针：指向my_string的地址
};

// 模块加载时:
// insmod mymodule.ko my_string="hello"
// 会调用: param_set_charp("hello", &my_param)
// 最终: my_string 指向新分配的 "hello" 字符串
