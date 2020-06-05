#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <juce_cryptography/juce_cryptography.h>
#include <juce_core/juce_core.h>

typedef struct {
    PyObject_HEAD
    juce::RSAKey rsa;
} PyRSAKey;

static PyTypeObject PyRSAKeyType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "juce_rsa.RSAKey",
    .tp_basicsize = sizeof(PyRSAKey),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "RSAKey object can be used to encypt/decrypt using RSA",
    .tp_new = PyType_GenericNew,
};

static PyObject * juce_rsa_create_key_pair(PyObject *self, PyObject *args)
{
    const char *val1 = nullptr;
    const char *val2 = nullptr;
    if (!PyArg_ParseTuple(args, "ss", &val1, &val2))
        return NULL;

    return PyLong_FromLong(0);
}


static PyMethodDef juceRsaMethods[] = {
    {"create_key_pair",  juce_rsa_create_key_pair, METH_VARARGS, "Create key pair."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef juce_rsa_module = {
    PyModuleDef_HEAD_INIT,
    .m_name = "juce_rsa",
    .m_doc = "Python bindings for juce::RSAKey.",
    .m_size = -1, /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    .m_methods = juceRsaMethods,
};

PyMODINIT_FUNC
PyInit_juce_rsa(void)
{
    PyObject *m;
    if (PyType_Ready(&PyRSAKeyType) < 0)
        return NULL;

    m = PyModule_Create(&juce_rsa_module);
    if (m == NULL)
        return NULL;

    Py_INCREF(&PyRSAKeyType);
    if (PyModule_AddObject(m, "RSAKey", (PyObject *) &PyRSAKeyType) < 0) {
        Py_DECREF(&PyRSAKeyType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}
