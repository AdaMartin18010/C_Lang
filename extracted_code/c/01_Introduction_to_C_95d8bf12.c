/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\01_Introduction_to_C.md
 * Line: 263
 * Language: c
 * Block ID: 95d8bf12
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Python C扩展示例
static PyObject* my_function(PyObject* self, PyObject* args) {
    int input;
    if (!PyArg_ParseTuple(args, "i", &input))
        return NULL;
    return PyLong_FromLong(input * 2);
}
