#include <Python.h>

#include <stdlib.h>
// #include <Python.h>

/* convert_ann_to_snn is a C++ wrapper to call the convert_ann_to_snn 
Python implementation of converting an analog neural network to a spiking
neural network. */
void convert_ann_to_snn(char *path_wd, char *model_name)
{
    /* TODO - Try copying the snntoolbox locally*/
    setenv("PYTHONPATH", ".", 1);
    Py_Initialize();

    /* First, import the ann_to_snn module */
    PyObject *ann_to_snn_module_string = PyUnicode_FromString((char *)"ann_to_snn");
    PyErr_Print();

    PyObject *ann_to_snn_module = PyImport_Import(ann_to_snn_module_string);
    PyErr_Print();
    /* Now, get a reference for the "convert_ann_to_snn" function */
    PyObject *convert_ann_to_snn_function = PyObject_GetAttrString(ann_to_snn_module, (char *)"convert_ann_to_snn");
    PyObject *args = PyTuple_Pack(2, PyUnicode_FromString(path_wd), PyUnicode_FromString(model_name));

    /* Run the function (convert_ann_to_snn has no return value) */
    PyObject *result = PyObject_CallObject(convert_ann_to_snn_function, args);

    Py_DECREF(ann_to_snn_module_string);
    Py_DECREF(ann_to_snn_module);
    Py_DECREF(convert_ann_to_snn_function);
    Py_DECREF(args);

    Py_Finalize();
}

/* create_sample_cnn is a C++ wrapper to generate a sample convolutional 
neural network. */
void create_sample_cnn(char *path_wd, char *model_name)
{
    /* Set the PYTHONPATH to the working directory */
    setenv("PYTHONPATH", ".", 1);
    Py_Initialize();

    /* Build the name object */
    PyObject *pName = PyUnicode_FromString((char *)"ann_to_snn");

    /* Load the module object */
    PyObject *pModule = PyImport_Import(pName);
    PyErr_Print();
    PyObject *pDict = PyModule_GetDict(pModule);

    PyObject *pFunc = PyDict_GetItemString(pDict, (char *)"create_sample_cnn");

    PyObject *pResult;
    if (PyCallable_Check(pFunc))
    {
        PyObject *args;
        pResult = PyObject_CallObject(pFunc, args);
        PyErr_Print();
    }
    else
    {
        PyErr_Print();
    }
    PyObject *pathObject = PySequence_GetItem(pResult, 0);
    PyObject *modelObject = PySequence_GetItem(pResult, 1);
    if (!pathObject)
    {
        printf("error\n");
    }
    if (!modelObject)
    {
        printf("error\n");
    }

    Py_ssize_t size;
    const char *ptr = PyUnicode_AsUTF8AndSize(pathObject, &size);
    const char *ptr2 = PyUnicode_AsUTF8AndSize(modelObject, &size);

    Py_DECREF(pModule);
    Py_DECREF(pName);

    Py_Finalize();
    return;
}

int main(int argc, char *argv[])
{
    create_sample_cnn((char *)"", (char *)"");
    //convert_ann_to_snn((char *)"/Users/jake/Code/ncc_prototype/temp/1594768532.239394", (char *)"mnist_cnn");
    return 0;
}