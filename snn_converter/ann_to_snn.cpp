#include <Python.h>

#include <stdlib.h>
#include <string.h>

/* convert_ann_to_snn is a C++ wrapper to call the convert_ann_to_snn 
Python implementation of converting an analog neural network to a spiking
neural network. */
void convert_ann_to_snn(char *path_wd, char *model_name)
{
    setenv("PYTHONPATH", ".", 1);
    Py_Initialize();

    /* First, import the ann_to_snn module */
    PyObject *ann_to_snn_module_string = PyUnicode_FromString((char *)"ann_to_snn");
    if (!ann_to_snn_module_string)
    {
        PyErr_Print();
        return;
    }

    PyObject *ann_to_snn_module = PyImport_Import(ann_to_snn_module_string);
    if (!ann_to_snn_module)
    {
        PyErr_Print();
        return;
    }
    /* Now, get a reference for the "convert_ann_to_snn" function */
    PyObject *convert_ann_to_snn_function = PyObject_GetAttrString(ann_to_snn_module, (char *)"convert_ann_to_snn");
    if (!convert_ann_to_snn_function)
    {
        PyErr_Print();
        return;
    }
    PyObject *args = PyTuple_Pack(2, PyUnicode_FromString(path_wd), PyUnicode_FromString(model_name));
    if (!args)
    {
        PyErr_Print();
        return;
    }
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
    if (!pName)
    {
        PyErr_Print();
        return;
    }

    /* Load the module object */
    PyObject *pModule = PyImport_Import(pName);
    if (!pModule)
    {
        PyErr_Print();
        return;
    }

    PyObject *pDict = PyModule_GetDict(pModule);
    if (!pDict)
    {
        PyErr_Print();
        return;
    }

    PyObject *pFunc = PyDict_GetItemString(pDict, (char *)"create_sample_cnn");
    if (!pFunc)
    {
        PyErr_Print();
        return;
    }

    /* Allocate object to store return value */
    PyObject *pResult;

    if (PyCallable_Check(pFunc))
    {
        /* Let args be empty since pFunc for "create_sample_cnn" does not accept arguments */
        PyObject *args;
        pResult = PyObject_CallObject(pFunc, args);
        if (!pResult)
        {
            PyErr_Print();
            return;
        }
    }
    else
    {
        PyErr_Print();
        return;
    }
    /* "create_sample_cnn" returns a tuple, so we need to break it down and extract the results */
    PyObject *pathObject = PySequence_GetItem(pResult, 0);
    if (!pathObject)
    {
        PyErr_Print();
        return;
    }

    PyObject *modelObject = PySequence_GetItem(pResult, 1);
    if (!modelObject)
    {
        PyErr_Print();
        return;
    }

    Py_ssize_t size;
    const char *path_wd_ptr = PyUnicode_AsUTF8AndSize(pathObject, &size);
    path_wd = (char *)malloc((size + 1) * sizeof(char));
    strcpy(path_wd, path_wd_ptr);
    printf("path_wd = %s\n", path_wd);

    const char *model_name_ptr = PyUnicode_AsUTF8AndSize(modelObject, &size);
    model_name = (char *)malloc((size + 1) * sizeof(char));
    strcpy(model_name, model_name_ptr);
    printf("model_name = %s\n", model_name);

    Py_DECREF(pModule);
    Py_DECREF(pName);

    Py_Finalize();
    return;
}

int main(int argc, char *argv[])
{
    char *path_wd;
    char *model_name;
    //create_sample_cnn(path_wd, model_name);
    //printf("path_wd = %s\n", path_wd);
    free(path_wd);
    free(model_name);
    convert_ann_to_snn((char *)"/Users/jake/Code/ncc_prototype/temp/1594768532.239394", (char *)"mnist_cnn");
    return 0;
}