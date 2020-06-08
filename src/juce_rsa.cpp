#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include <juce_cryptography/juce_cryptography.h>
#include <juce_core/juce_core.h>

typedef struct
{
    PyObject_HEAD
        juce::RSAKey *rsa = nullptr;
} PyRSAKey;

static void
PyRSAKey_dealloc(PyRSAKey *self)
{
    delete self->rsa;
    self->rsa = nullptr;
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *
PyRSAKey_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyRSAKey *self;
    self = (PyRSAKey *)type->tp_alloc(type, 0);
    if (self != NULL)
    {
        self->rsa = new juce::RSAKey();
    }
    return (PyObject *)self;
}

static int
PyRSAKey_init(PyRSAKey *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = { (char*) "s", NULL};
    const char *string_repr = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|s", kwlist, &string_repr))
        return -1;

    if (string_repr)
    {
        juce::String jstring(string_repr);
        if (!jstring.containsChar(','))
        {
            PyErr_SetString(PyExc_ValueError,
                            "String representation must be two comma-separated hex numbers");
            return -1;
        }
        *(self->rsa) = juce::RSAKey(string_repr);
    }
    return 0;
}

static PyObject *
PyRSAKey_apply(PyRSAKey *self, PyObject *const *params, Py_ssize_t count)
{
    if (count != 1)
    {
        PyErr_SetString(PyExc_TypeError, "Apply needs one argument");
        return NULL;
    }
    auto obj = params[0];
    juce::BigInteger bigInt;
    PyObject *result = nullptr;

    if (PyUnicode_Check(obj))
    {
        bigInt.parseString(PyUnicode_AsUTF8(obj), 16);
        if (!self->rsa->applyToValue(bigInt))
        {
            PyErr_SetString(PyExc_AssertionError, "Using an uninitialized key");
            return NULL;
        }
        auto jStr = bigInt.toString(16);
        result = PyUnicode_DecodeUTF8(jStr.toRawUTF8(), jStr.getNumBytesAsUTF8(), "strict");
    }
    else if (PyLong_Check(obj))
    {
        const auto bits = _PyLong_NumBits(obj);
        // Always allocate extra to extend with zero
        size_t numBytes = (bits >> 3) + 1;
        juce::MemoryBlock memBlock(numBytes, true);
        unsigned char *data = static_cast<unsigned char *>(memBlock.getData());
        const int little_endian = 1;
        const int is_signed = 0;
        PyLongObject *v = reinterpret_cast<PyLongObject *>(obj);
        if (_PyLong_AsByteArray(v, data, memBlock.getSize(), little_endian, is_signed) == -1)
        {
            PyErr_SetString(PyExc_ValueError, "Could not convert to integer");
            return NULL;
        }
        bigInt.loadFromMemoryBlock(memBlock);
        if (!self->rsa->applyToValue(bigInt))
        {
            PyErr_SetString(PyExc_AssertionError, "Using an uninitialized key");
            return NULL;
        }

        memBlock = bigInt.toMemoryBlock();
        data = static_cast<unsigned char *>(memBlock.getData());
        result = _PyLong_FromByteArray(data, memBlock.getSize(), little_endian, is_signed);
    }
    else
    {
        PyErr_SetString(PyExc_TypeError, "Hex string or integer expected");
        return NULL;
    }

    return result;
}

static PyMethodDef PyRSAKey_methods[] = {
    {"apply", (PyCFunction) PyRSAKey_apply, METH_FASTCALL,
     "Apply RSA to a value"},
    {NULL} /* Sentinel */
};

static PyTypeObject PyRSAKeyType = {
    PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "juce_rsa.RSAKey",
    .tp_basicsize = sizeof(PyRSAKey),
    .tp_itemsize = 0,
    .tp_dealloc = (destructor)PyRSAKey_dealloc,
    .tp_repr = (reprfunc)nullptr,
    .tp_str = (reprfunc)nullptr,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_doc = "RSAKey object can be used to encypt/decrypt using RSA",
    .tp_methods = PyRSAKey_methods,
    .tp_init = (initproc)PyRSAKey_init,
    .tp_new = PyRSAKey_new,
};

static PyObject *juce_rsa_create_key_pair(PyObject *self, PyObject *args)
{
    const char *val1 = nullptr;
    const char *val2 = nullptr;
    if (!PyArg_ParseTuple(args, "ss", &val1, &val2))
        return NULL;

    return PyLong_FromLong(0);
}

static PyMethodDef juceRsaMethods[] = {
    {"create_key_pair", juce_rsa_create_key_pair, METH_VARARGS, "Create key pair."},
    {NULL, NULL, 0, NULL} /* Sentinel */
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
    if (PyModule_AddObject(m, "RSAKey", (PyObject *)&PyRSAKeyType) < 0)
    {
        Py_DECREF(&PyRSAKeyType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}
