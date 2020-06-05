#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <juce_cryptography/juce_cryptography.h>
#include <juce_core/juce_core.h>

static PyObject * juce_rsa_apply(PyObject *self, PyObject *args)
{
    const char *val1 = nullptr;
    const char *val2 = nullptr;
    if (!PyArg_ParseTuple(args, "s", &val1, &val2))
        return NULL;

    juce::RSAKey rsa;
    return PyLong_FromLong(0);
}


static PyMethodDef juceRsaMethods[] = {
    {"apply",  juce_rsa_apply, METH_VARARGS, "Apply RSA."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef juce_rsa_module = {
    PyModuleDef_HEAD_INIT,
    "juce_rsa",   /* name of module */
    NULL,      /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    juceRsaMethods
};


PyMODINIT_FUNC
PyInit_juce_rsa(void)
{
    PyObject *m;

    m = PyModule_Create(&juce_rsa_module);
    if (m == NULL)
        return NULL;

    return m;
}
